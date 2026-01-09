#include <chrono>
#include <thread>
#include <cmath>

#define DEBUG_PRINTS
#include "polygon.h"
#include "utils.h"

namespace polygon_ns
{

using graphics_ns::graphics;
using std::cout;
using std::endl;
using std::ios;
using std::log2;

vector_3 view(0, 0, -1000000);
vector_3 n_light(0, 0, -1024);
polygon::poly_list polygon::_draw_list;

polygon::polygon(graphics& gfx)
{
	_gfx = &gfx;

	_clear = _gfx->get_color_val(graphics::color_idx::black);
	_gfx->set_alpha(_clear, _gfx->get_alpha_val(graphics::alpha_idx::A16));

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

	while (!f.eof() && !finish) {
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
					sys_error("polygon::read allocation error -  polygon");
				}
			}
			else {
				sys_error("polygon::read error polygon");
			}
			break;
		case 'c':
			len = read_word(f, line);
			if (len) {
				_color = atoi(line);
				_argb = _gfx->get_color_val(static_cast<graphics::color_idx>(_color));
			}
			else {
				sys_error("polygon::read error polygon");
			}
			break;
		case 'f':
			len = read_word(f, line);
			if (len) {
				_force = atoi(line);
			}
			else {
				sys_error("polygon::read error polygon");
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
					delete v;
					sys_error("polygon::read error polygon");
				}
			}
			else {
				sys_error("element::read allocation error -  polygon");
			}
			break;
		default:
			finish = 1;
			f.seekg(-4, ios::cur);
			break;
		}
	}

	if (!_name) {
		sys_error("polygon: no name");
	}

	if (_points.size() < 3) {
		sys_error("polygon: not enough vectors");
	}

	_fill = find_fill();
	// _normal = find_normal();

	return true;
}

void polygon::print() const
{
#ifdef DEBUG_PRINTS
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
#endif // DEBUG_PRINTS
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

void polygon::make_color(unit u [[maybe_unused]])
{
#if 0
	u.print();

	if (u > unit_ns::UNIT) {
		u = unit_ns::UNIT;
	}

	uint32_t alpha = static_cast<uint32_t>((long)u);
	// DBG("make_color: " << *_name << STR(" alpha: ", 1) << HEX(alpha, 2));

	// verify alpha is in range
	int shift_amount = static_cast<int>(std::round(log2(1024.0f / _gfx->get_num_alphas())));
	// DBG("make_color: " << *_name << STR(" shift_amount: ", 1) << shift_amount);
	if (shift_amount < 0) {
		shift_amount = 0; // guard against negative shifts
	}
	alpha >>= shift_amount;
	// DBG("make_color: " << *_name << STR(" alpha: ", 1) << HEX(alpha, 2));

	if (alpha) {
		alpha--;
	}
	// DBG("make_color: " << *_name << STR(" alpha: ", 1) << HEX(alpha, 2));

	uint32_t a = _gfx->get_alpha_val(static_cast<graphics_ns::graphics::alpha_idx>(alpha));
#else
	uint32_t a = _gfx->get_alpha_val(graphics_ns::graphics::A16);
#endif // 0
	DBG("make_color: " << *_name << " a: " << HEX(a, 2));
	_gfx->set_alpha(_argb, a);
	DBG("polygon: make_color: argb: " << *_name << STR(": ", 1) << HEX(_argb.c, 8));
}

void polygon::update(matrix& m_gen, matrix& m_rot)
{
	vector_3 dist, fill, normal;
	unit view_angle, light_angle;

	DBG("update: " << *_name);
	fill = m_gen * _fill;
	normal = m_rot * _normal;
	dist = fill - view;
	view_angle = normal * dist;
	if ((view_angle < unit_ns::ZERO) || _force) {
		DBG("view_angle < unit_ns::ZERO");
		light_angle = normal * n_light;
		if ((light_angle > unit_ns::ZERO) || _force) {
			DBG("light_angle > unit_ns::ZERO");
			_depth = fill.get(Z_);
			_draw_mat = m_gen;
			make_color(abs(light_angle));
			_draw_list.push_back(this);
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
	if (_draw_list.size() > 1) {
		_draw_list.sort(compare_depth);
	}
}

void polygon::show(frame_buffer& fb)
{
	DBG("polygon: show: argb: " << *_name << STR(": ", 1) << HEX(_argb.c, 8));
	_gfx->fill_buffer(fb, _argb);
}

void polygon::show_all()
{
	if (_draw_list.empty()) {
		DBG("show_all: empty");
		return;
	}

	frame_buffer fb = _gfx->get_clear_backbuffer(_clear);

	DBG("walk list");
	while (_draw_list.size()) {
		polygon* poly = _draw_list.front();
		_draw_list.pop_front();
		poly->show(fb);
	}

	_gfx->present();
}

void polygon::sort_polygons()
{
	polygon tmp;
	tmp.sort();
}

void polygon::show_polygons()
{
	polygon tmp;
	tmp.show_all();
}

} // namespace polygon_ns
