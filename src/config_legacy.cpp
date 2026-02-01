#include <stdexcept>
#include <string>
#include <cctype>

#include "common.h"
#include "config_legacy.h"
#include "utils.h"

namespace config_ns
{

bool parse_polygon(polygon_def& pd, std::ifstream& ifs)
{
	LINE line;
	bool rc, finish = false;
	vector_3 v;
	vector_3 n;
	int len;

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

	return true;
}

bool parse_element(element_def& ed, std::ifstream& ifs)
{
	LINE line;
	bool rc, finish = false;
	int len;

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
			rc = ed.att.read(ifs);
			if (!rc) {
				ERR("parse_element: bad attrib");
				return false;
			}
			break;
		case 'f':
			// element active flag
			len = read_word(ifs, line);
			if (!len) {
				sys_error("element::read error flag");
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

	return true;
}

document parse_legacy()
{
	document doc;
	polygon_def pd;
	element_def ed;
	LINE line;
	bool rc;
	char filename[] = "box.dat";
	std::ifstream ifs;

	ifs.open(filename, std::ios::in);
	if (!ifs) {
		sys_error("file not found:", filename);
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
			rc = parse_polygon(pd, ifs);
			if (!rc) {
				sys_error("parse_polygon: failed to parse");
			}
			doc.polygons.push_back(std::move(pd));
			break;
		case 'e':
			rc = parse_element(ed, ifs);
			if (!rc) {
				sys_error("parse_element: failed to parse");
			}
			doc.elements.push_back(std::move(ed));
			break;
		}
	}

	ifs.close();
	return doc;
}

} // namespace config_ns
