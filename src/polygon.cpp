#include "polygon.h"
#include "utils.h"

namespace polygon_ns
{

using std::cout;
using std::endl;
using std::ios;

vector_3 view(0, 0, -1000000);
vector_3 n_light(0, 0, -1024);
polygon::poly_list polygon::_draw_list;

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
			break;
		case 'v':
			v = new vector_3;
			if (v) {
				if (v->read(f)) {
					_points.push_back(v);
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

	if (!_name) {
		error("polygon: no name");
	}

	if (_points.size() < 3) {
		error("polygon: not enough vectors");
	}

	_fill = find_fill();
	// _normal = find_normal();

	return ret;
}

void polygon::print() const
{
#ifdef DEBUG_GRFX
	DBG("      polygon:");
	DBG(STR("        name: ", 1) << *_name);
	DBG(STR("        force: ", 1) << DEC(_force, 4));
	DBG(STR("        color: ", 1) << DEC((int)_color, 4));
	DBG(STR("        fill:", 1));
	_fill.print();
	DBG(STR("        normal:", 1));
	_normal.print();
	DBG(STR("        draw color: ", 1) << DEC((int)_draw_color, 4));
	DBG(STR("        depth:", 1));
	_depth.print();
	if (!_points.empty()) {
		DBG("        points:");
		for (const auto vec : _points) {
			DBG(STR("        point:", 1));
			vec->print();
		}
	}
#endif // DEBUG_GRFX
}

vector_3 polygon::find_fill()
{
	vector_3 v;
	long num = 0;

	for (const auto vec : _points) {
		v += *vec;
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
	vector_3 *v1, *v2, v;

	if (_points.size() > 2) {
		v1 = _points.front();
		v2 = _points.back();

		v.get(X_) = (v1->get(Y_) * v2->get(Z_) -
		             v1->get(Z_) * v2->get(Y_));
		v.get(Y_) = (v1->get(Z_) * v2->get(X_) -
		             v1->get(X_) * v2->get(Z_));
		v.get(Z_) = (v1->get(X_) * v2->get(Y_) -
		             v1->get(Y_) * v2->get(X_));

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
	vector_3 dist, fill, normal;
	unit view_angle, light_angle;

	fill = m_gen * _fill;
	normal = m_rot * _normal;
	dist = fill - view;
	view_angle = normal * dist;
	if ((view_angle < unit_ns::ZERO) || _force) {
		light_angle = normal * n_light;
		if ((light_angle > unit_ns::ZERO) || _force) {
			// _depth = dist * dist;
			_depth = fill.get(Z_);
			_draw_mat = m_gen;
			_draw_color = make_color(_color, abs(light_angle));
			_draw_list.push_front(this);
		}
	}
}

void polygon::clear()
{
	_draw_list.clear();
}

static bool compare_depth(polygon* p1, polygon* p2)
{
	long depth1 = (long)p1->get_depth();
	long depth2 = (long)p2->get_depth();
	bool ret = (depth1 < depth2);
	DBG(STR("depth1: ", 1) << depth1 << STR(" depth2: ", 1) << depth2 << STR(" ret: ", 1) << ret);
	return ret;
}

void polygon::sort()
{
	if (_draw_list.empty()) {
		DBG("sort: empty");
		return;
	}

	_draw_list.sort(compare_depth);
}

void polygon::show_all()
{
	DBG("walk list");
	if (_draw_list.empty()) {
		DBG("show_all: empty");
		return;
	}

	while (_draw_list.size()) {
		polygon* poly = _draw_list.front();
		_draw_list.pop_front();
		poly->print();
	}
	// delay(17);
}

} // namespace polygon_ns
