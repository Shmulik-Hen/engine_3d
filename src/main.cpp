#include <chrono>
#include <thread>
#include <filesystem>
#include <getopt.h>

// #define DEBUG_PRINTS
#include "common.h"
#include "scene.h"
#include "utils.h"

using namespace attrib_ns;
using namespace element_ns;
using namespace graphics_ns;
using namespace scene_ns;
using namespace config_ns;
using attrib = attrib_ns::attrib;
using element = element_ns::element;
using graphics = graphics_ns::graphics;
using input_state = graphics_ns::graphics::input_state;
namespace fs = std::filesystem;

const std::string version = "0.9";
const std::string def_conf_name = "box";
const std::string def_conf_path = "../config/";
const std::string def_file_name = "config";
#ifdef USING_JSON
const std::string ext = ".json";
#else
const std::string ext = ".dat";
#endif
const std::string def_conf_file = def_conf_path + def_file_name + ext;

void usage(std::string str)
{
	// clang-format off
	fprintf(stdout, "\nUsage: %s [OPTIONS]... [CONFIG_FILE] [CONFIG_NAME]\n", str.c_str());
	fprintf(stdout, "Render animated 3D objects\n\n");
	fprintf(stdout, "OPTIONS:\n");
	fprintf(stdout, "  -c, --conf <config name>\tspecify a configuration name to choose\n");
	fprintf(stdout, "  -f, --file <config file>\tspecify a JSON file name to read the configuration from\n");
	fprintf(stdout, "  -h, --help\t\t\tdisplay this help and exit\n");
	fprintf(stdout, "  -u, --usage\t\t\tdisplay this help and exit\n");
	fprintf(stdout, "  -v, --version\t\t\toutput version information and exit\n");
	fprintf(stdout, "\n");
	// clang-format on
}

int main(int argc, char** argv)
{
	try {
		// Define the long/short options
		const char* shortops = "c:f:huv";
		// clang-format off
		const struct option longopts[] = {
			{ "conf",    required_argument, 0, 'c'},
			{ "file",    required_argument, 0, 'f'},
			{ "help",    no_argument,       0, 'h'},
			{ "usage",   no_argument,       0, 'u'},
			{ "version", no_argument,       0, 'v'},
			{ 0, 0, 0, 0}  // Terminator
		};
		// clang-format on

		std::string filename;
		std::string conf_name;
		std::string given_file;
		std::string given_conf;
		std::string full_path = argv[0];
		fs::path p(full_path);
		std::string path = p.parent_path().string();
		std::string prog = p.filename().string();
		int index;
		int c;

		while ((c = getopt_long(argc, argv, shortops, longopts, &index)) != -1) {
			DBG("getopt() returned '" << (char)c << "' index:" << index);
			switch (c) {
			case 'c':
				given_conf = optarg;
				DBG("got a configuration name: " << given_conf);
				break;
			case 'f':
				given_file = optarg;
				DBG("got a configuration file: " << given_file);
				break;
			case 'h':
			case 'u':
			case '?':
				usage(prog);
				return 0;
			case 'v':
				std::cout << version << std::endl;
				return 0;
			default:
				std::cerr << "?? getopt_long returned character code 0" << std::oct << c << std::endl;
				usage(prog);
				return 0;
			}
		}

		// Process any remaining non-option arguments
		if (optind < argc) {
			DBG("Non-option arguments: ");
			while (optind < argc) {
				DBG(rgv[optind]);
				if (!given_file.length()) {
					given_file = argv[optind];
					DBG("got a configuration file: " << given_file);
				}
				else if (!given_conf.length()) {
					given_conf = argv[optind];
					DBG("got a configuration name: " << given_conf);
				}
				optind++;
			}
			DBG("");
		}

		if (argc == 1) {
			filename = def_conf_file;
			conf_name = def_conf_name;
			DBG("  using default config file '" << filename << "'");
			DBG("  using default config name '" << conf_name << "'");
		}
		else {
			if (given_file.length()) {
				filename = given_file;
				DBG("  using config file '" << filename << "'");
			}
			else {
				filename = def_conf_file;
				DBG("  using default config file '" << filename << "'");
			}

			if (given_conf.length()) {
				conf_name = given_conf;
				DBG("  using config name '" << conf_name << "'");
			}
			else {
				conf_name = def_conf_name;
				DBG("  using default config name '" << conf_name << "'");
			}
		}

		std::unique_ptr<my_scene> scene_unique = std::make_unique<my_scene>();
		my_scene* scene = scene_unique.get();

		if (!(scene &&
		      scene->frame_ctx.draw_vec.get() &&
		      scene->frame_ctx.state.get() &&
		      scene->frame_ctx.state->grfx.gfx.get())) {
			sys_error("failed to initialize scene");
		}

		scene->parse(filename, conf_name);
		scene->build();

		if (!scene->root) {
			sys_error("root is null");
		}

		if (scene->poly_list.empty()) {
			sys_error("no polygons");
		}

		// find the relevant elements to work with
		const std::string world_name = "world";
		const std::string box_name = "box";

		element* world = scene->root->find(scene->root, world_name);
		if (!world) {
			sys_error("find world failed");
		}

		element* box = scene->root->find(scene->root, box_name);
		if (!box) {
			sys_error("find box failed");
		}

#ifdef DEBUG_PRINTS
		DBG("initial:");
		DBG("print tree");
		scene->root->print_all();
		DBG("walk list");
		for (const auto poly : scene->poly_list) {
			poly->print();
		}
#endif // DEBUG_PRINTS

		// attrib(rotationX, rotationY, rotationZ, positionX, positionY, positionZ, zoom)
		attrib initial_att(45, 0, 45, 0, 0, 0, 1);
		attrib keep_moving(0, 1, 0, 0, 0, 0, 1);

		world->update(initial_att);

		input_state in {};
		graphics* gfx = scene->frame_ctx.state->grfx.gfx.get();

		while (!in.quit) {
			gfx->poll_events(in);
			DBG("in.quit: " << in.quit << " in.esc: " << in.key_escape);

			scene->update();
			scene->render();

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			box->update(keep_moving);
		}

		return 0;
	}
	catch (const std::exception& e) {
		std::fprintf(stderr, "Fatal: %s\n", e.what());
		return 1;
	}
}
