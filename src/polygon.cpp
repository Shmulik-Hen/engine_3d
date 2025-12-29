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

vector_3 view(0, 0, -1000000);
vector_3 n_light(0, 0, -1024);

polygon::polygon()
{
	_points.clear();
}

polygon::~polygon()
{
	if (_name) {
		delete _name;
	}
}

bool polygon::read(ifstream& f)
{
	LINE line;
	vector_3* v;
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
			DBG(STR("read: normal:", 1));
			// _normal.print();
			break;
		case 'v':
			v = new vector_3;
			if (v) {
				if (v->read(f)) {
					_points.push_front(*v);
					DBG(STR("read: vector:", 1));
					// v->print();
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

	DBG(STR("read: fill: before", 1));
	// _fill.print();
	_fill = find_fill();
	DBG(STR("read: fill: after:", 1));
	// _fill.print();
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
	if (!_points.empty()) {
		DBG("        points:");
		for (vec_it it = _points.cbegin(); it != _points.cend(); ++it) {
			const vector_3* v = &*it;
			if (v) {
				DBG(STR("        point:", 1));
				v->print();
			}
		}
	}
}

vector_3 polygon::find_fill()
{
	vector_3 v;
	long num = 0;

	for (vec_it it = _points.cbegin(); it != _points.cend(); ++it) {
		v += *it;
		num++;
	}

	unit n(num << 10);
	v.get(X_) /= n;
	v.get(Y_) /= n;
	v.get(Z_) /= n;

	return v;
}

vector_3 polygon::find_normal()
{
	vec_it it = _points.begin();
	vector_3 v1, v2, v;

	if (_points.size() >= 2) {
		v1 = *it;
		v2 = *++it;

		v.get(X_) = (v1.get(Y_) * v2.get(Z_) -
		             v1.get(Z_) * v2.get(Y_));
		v.get(Y_) = (v1.get(Z_) * v2.get(X_) -
		             v1.get(X_) * v2.get(Z_));
		v.get(Z_) = (v1.get(X_) * v2.get(Y_) -
		             v1.get(Y_) * v2.get(X_));

		normalize(v);
	}

	return v;
}

char make_color(char c1, unit c2)
{
	long color = (long)c2;
	if (color >= 1024) {
		color = 1023;
	}
	return (char)(((color >> 2) & 0x00f0) | (c1 & 0x0F));
}

void polygon::update(matrix& m_gen, matrix& m_rot)
{
	vector_3 dist;
	unit view_angle, light_angle;

	DBG(STR("polygon name: ", 1) << *_name);
	DBG("fill: before");
	// _fill.print();
	_fill = m_gen * _fill;
	DBG("fill: after");
	// _fill.print();
	DBG("normal: before");
	// _normal.print();
	_normal = m_rot * _normal;
	DBG("normal: after");
	// _normal.print();

	DBG("dist: before");
	// dist.print();
	dist = _fill - view;
	DBG("dist: after");
	// dist.print();
	view_angle = _normal * dist;
	if ((view_angle < unit_ns::ZERO) || _force) {
		light_angle = _normal * n_light;
		if ((light_angle > unit_ns::ZERO) || _force) {
			DBG("depth: before");
			// _depth.print();
			_depth = dist * dist;
			DBG("depth: after");
			// _depth.print();
			_color = make_color(_color, abs(light_angle));
		}
	}
}

int operator<(const polygon& p1, const polygon& p2)
{
	return (p1._depth > p2._depth);
}

} // namespace polygon_ns
