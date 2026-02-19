#include <stdexcept>
#include <string>
#include <cctype>

// #define DEBUG_PRINTS
#include "common.h"
#include "config_legacy.h"
#include "utils.h"

namespace config_ns
{

bool parse_polygon_legacy(AST& ast, std::ifstream& ifs)
{
	LINE line;
	bool rc, finish = false;
	vector_3 v;
	vector_3 n;
	int len;
	polygon_def pd {};

	while (!ifs.eof() && !finish) {
		auto pos = ifs.tellg();
		// seek next token/marker
		while ((!read_word(ifs, line)) && (!ifs.eof()));
		if (ifs.eof()) {
			break;
		}

		switch (line[1]) {
		case 'n':
			len = read_word(ifs, line);
			if (!len) {
				ERR("parse_polygon: missing name");
				return false;
			}
			pd.name = line;
			break;
		case 'c':
			len = read_word(ifs, line);
			if (!len) {
				ERR("parse_polygon: missing color");
				return false;
			}
			pd.color_index = std::atoi(line);
			break;
		case 'f':
			len = read_word(ifs, line);
			if (!len) {
				ERR("parse_polygon: missing force");
				return false;
			}
			pd.force = std::atoi(line);
			break;
		case 'o':
			rc = n.read(ifs);
			if (!rc) {
				ERR("parse_polygon: bad normal");
				return false;
			}
			pd.normal_cfg = n;
			break;
		case 'v':
			rc = v.read(ifs);
			if (!rc) {
				ERR("parse_polygon: bad vertex");
				return false;
			}
			pd.points.push_back(v);
			break;
		default:
			// next section starts: rewind token and exit polygon parse
			finish = true;
			ifs.seekg(pos);
			break;
		}
	}

	ast.polygons.push_back(std::move(pd));
	return true;
}

bool parse_element_legacy(AST& ast, std::ifstream& ifs)
{
	LINE line;
	bool rc, finish = false;
	int len;
	element_def ed {};

	while (!ifs.eof() && !finish) {
		auto pos = ifs.tellg();
		// seek next token/marker
		while ((!read_word(ifs, line)) && (!ifs.eof()));
		if (ifs.eof()) {
			break;
		}

		switch (line[1]) {
		case 'n':
			len = read_word(ifs, line);
			if (!len) {
				ERR("parse_element: missing name");
				return false;
			}
			ed.name = line;
			break;
		case 't':
			len = read_word(ifs, line);
			if (len) {
				// can be missing in case of root ("world")
				ed.parent = line;
			}
			break;
		case 'a':
			// adapt to your attrib format; if attrib already has read():
			rc = ed.ini_att.read(ifs);
			if (!rc) {
				ERR("parse_element: bad attrib");
				return false;
			}
			break;
		case 'f':
			// element active flag
			len = read_word(ifs, line);
			if (!len) {
				sys_error("parse_element: bad flag");
			}

			ed.active = (bool)atoi(line);
			break;
		case 'p':
			len = read_word(ifs, line);
			if (len) {
				ed.polygons.emplace_back(line);
			}
			break;
		default:
			// next section starts: rewind token and exit polygon parse
			finish = true;
			ifs.seekg(pos);
			break;
		}
	}

	ast.elements.push_back(std::move(ed));
	return true;
}

AST parse_legacy(const std::string& filename, const std::string& conf_name [[maybe_unused]])
{
	AST ast;
	LINE line;
	bool rc;
	std::ifstream ifs;

	ifs.open(filename.c_str(), std::ios::in);
	if (!ifs) {
		sys_error("parse_legacy: file not found:", filename.c_str());
	}

	ifs.unsetf(std::ios::skipws);

	while (!ifs.eof()) {
		// seek next token/marker
		while ((!read_word(ifs, line)) && (!ifs.eof()));
		if (ifs.eof()) {
			break;
		}

		switch (line[1]) {
		case 'p':
			// parse until next section
			rc = parse_polygon_legacy(ast, ifs);
			if (!rc) {
				sys_error("parse_polygon_legacy: failed to parse");
			}
			break;
		case 'e':
			rc = parse_element_legacy(ast, ifs);
			if (!rc) {
				sys_error("parse_element_legacy: failed to parse");
			}
			break;
		}
	}

	ifs.close();
	return ast;
}

} // namespace config_ns
