#include <iostream>
#include <ios>
#include <iterator>
#include <stdlib.h>
#include "polygon.h"
#include "utils.h"

namespace polygon_ns
{

using std::cout;
using std::endl;
using std::ios;

polygon::polygon()
{
	_points = new vec_list;
	_points->clear();
}

polygon::~polygon()
{
	if (_points) {
		delete _points;
	}

	if (_name) {
		delete _name;
	}
}

bool polygon::read(ifstream& f)
{
	LINE line;
	my_vector* v;
	int finish = 0, len;
	bool rc, ret = true;

	while (!f.eof() && !finish) {
		rc = true;
		while ((!read_word(f, line)) && (!f.eof()));

		if (f.eof()) {
			break;
		}

		switch (line[1]) {
		case 'n':
			len = read_word(f, line);
			if (len) {
				_name = new string(line);
				if (!_name) {
					ERR("polygon::read allocation error -  polygon");
					rc = false;
				}
			}
			else {
				ERR("polygon::read error polygon");
				rc = false;
			}
			break;
		case 'c':
			len = read_word(f, line);
			if (len) {
				_color = atoi(line);
			}
			else {
				ERR("polygon::read error polygon");
				rc = false;
			}
			break;
		case 'f':
			len = read_word(f, line);
			if (len) {
				_force = atoi(line);
			}
			else {
				ERR("polygon::read error polygon");
				rc = false;
			}
			break;
		case 'o':
			_normal.read(f);
			break;
		case 'v':
			v = new my_vector;
			if (v) {
				if (v->read(f)) {
					_points->push_front(*v);
				}
				else {
					ERR("polygon::read error polygon");
					rc = false;
				}
			}
			else {
				ERR("element::read allocation error -  polygon");
				rc = false;
			}
			break;
		default:
			finish = 1;
			f.seekg(-4, ios::cur);
			break;
		}

		if (!rc) {
			ERR("polygon::read parsing error");
			ret = false;
		}
	}

	_fill = find_fill();
	// _normal = find_normal();

	if (!_name) {
		_name = new string("");
	}

	return ret;
}

void polygon::print() const
{
	DBG("      polygon:");
	DBG(STR("        name: ", 1) << *_name);
	DBG(STR("        force: ", 1) << DEC(_force, 4));
	DBG(STR("        color: ", 1) << DEC((int)_color, 4));
	DBG(STR("        depth:", 1));
	_depth.print();
	DBG(STR("        fill:", 1));
	_fill.print();
	DBG(STR("        normal:", 1));
	_normal.print();
	if (!_points->empty()) {
		DBG("        points:");
		for (vec_it it = _points->cbegin(); it != _points->cend(); ++it) {
			const my_vector* v = &*it;
			if (v) {
				v->print();
			}
		}
	}
}

my_vector polygon::find_fill()
{
	my_vector v;
	long num = 0;

	for (vec_it it = _points->cbegin(); it != _points->cend(); ++it) {
		v += *it;
		num++;
	}

	unit n(num << 10);
	v.get_coord(coord::X) /= n;
	v.get_coord(coord::Y) /= n;
	v.get_coord(coord::Z) /= n;

	return v;
}

my_vector polygon::find_normal()
{
	vec_it it = _points->begin();
	my_vector v1, v2, v;

	if (_points->size() >= 2) {
		v1 = *it;
		v2 = *++it;
	}

	v.get_coord(coord::X) = (v1.get_coord(coord::Y) * v2.get_coord(coord::Z) -
	                         v1.get_coord(coord::Z) * v2.get_coord(coord::Y));
	v.get_coord(coord::Y) = (v1.get_coord(coord::Z) * v2.get_coord(coord::X) -
	                         v1.get_coord(coord::X) * v2.get_coord(coord::Z));
	v.get_coord(coord::Z) = (v1.get_coord(coord::X) * v2.get_coord(coord::Y) -
	                         v1.get_coord(coord::Y) * v2.get_coord(coord::X));

	normalize(v);

	return v;
}

int operator<(const polygon& p1, const polygon& p2)
{
	return (p1._depth > p2._depth);
}

} // namespace polygon_ns
