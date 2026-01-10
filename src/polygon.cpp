#include <chrono>
#include <thread>
#include <cmath>

// #define DEBUG_PRINTS
#include "polygon.h"
#include "utils.h"

namespace polygon_ns
{

using graphics_ns::graphics;
using std::cout;
using std::endl;
using std::ios;
using std::log2;
using std::max;
using std::min;

enum
{
	OFF = 0,
	ON
};

vector_3 view(0, 0, -1000000);
vector_3 n_light(0, 0, -1024);
polygon::poly_list polygon::_draw_list;

polygon::drawing::drawing(graphics& gfx)
{
	_gfx = &gfx;

	_clear = _gfx->get_color_val(graphics::color_idx::black);
	// Use fully transparent black for edge color (ARGB 0x00000000), distinct from `_clear`
	// which is the standard (typically opaque) black returned by the graphics API. This
	// allows edge pixels to be treated specially (e.g., skipped in blending) while the
	// clear color remains the normal background black.
	_edge.c = 0x00000000;
	set_min_max();
}

frame_buffer polygon::drawing::clear()
{
	return _gfx->get_clear_backbuffer(_clear);
}

void polygon::drawing::set_color(int idx)
{
	_argb = _gfx->get_color_val(static_cast<graphics::color_idx>(idx));
}

void polygon::drawing::make_color(unit u, matrix& mgen)
{
	_draw_mat = mgen;

	u.print();

	if (u > unit_ns::UNIT) {
		u = unit_ns::UNIT;
	}

	unit u_base = unit_ns::UNIT;
	long val = (long)u;
	long base = (long)u_base;
	float factor = (float)val / (float)base;
	_draw_argb.c = _argb.c;
	_draw_argb.channel.r = (uint8_t)std::roundf((float)_argb.channel.r * factor);
	_draw_argb.channel.g = (uint8_t)std::roundf((float)_argb.channel.g * factor);
	_draw_argb.channel.b = (uint8_t)std::roundf((float)_argb.channel.b * factor);
}

void polygon::drawing::clear_scratch_pad()
{
	_scratch_pad.clear();
}

void polygon::drawing::prepare(const vector_3& original)
{
	vector_3 transformed, projected;

	transformed = _draw_mat * original;
	if (transformed.get(Z_) <= view.get(Z_)) {
		return;
	}

	projected = project(transformed, view);
	int x = (int)projected.get(X_) + (int)_vp_mid_pos.x;
	if (x > (int)_vp_max_pos.x || x < (int)_vp_min_pos.x) {
		return;
	}

	int y = -(int)projected.get(Y_) + (int)_vp_mid_pos.y;
	if (y > (int)_vp_max_pos.y || y < (int)_vp_min_pos.y) {
		return;
	}

	graphics::point p {static_cast<uint32_t>(x), static_cast<uint32_t>(y)};
	_scratch_pad.push_back(p);
}

void polygon::drawing::draw(frame_buffer& fb)
{
	uint32_t start_x, end_x;
	bool pix_on;
	graphics::point prev_pos, curr_pos, origin_pos, min_pos, max_pos;

	if (_scratch_pad.size() < 3) {
		return;
	}

	origin_pos = _scratch_pad.front();
	_scratch_pad.pop_front();

	min_pos = max_pos = prev_pos = origin_pos;

	// tracing the outer lines with the edge color
	while (_scratch_pad.size()) {
		curr_pos = _scratch_pad.front();
		_scratch_pad.pop_front();
		line(prev_pos, curr_pos, _edge, fb);
		if (prev_pos.x > curr_pos.x) {
			min_pos.x = curr_pos.x;
		}
		if (max_pos.x < curr_pos.x) {
			max_pos.x = curr_pos.x;
		}
		if (min_pos.y > curr_pos.y) {
			min_pos.y = curr_pos.y;
		}
		if (max_pos.y < curr_pos.y) {
			max_pos.y = curr_pos.y;
		}
		prev_pos = curr_pos;
	}
	line(prev_pos, origin_pos, _edge, fb);

	if (!is_in_bounds(min_pos, max_pos)) {
		return;
	}

	adjust_min(min_pos);
	adjust_max(max_pos);

	// filling the area with the relevant color
	for (uint32_t y = min_pos.y; y <= max_pos.y; y++) {
		pix_on = OFF;
		for (uint32_t x = min_pos.x; x <= max_pos.x; x++) {
			if (getpixel({x, y}, fb) == _edge) {
				if (!pix_on) {
					end_x = start_x = x;
				}
				else {
					end_x = x;
				}
				pix_on = ON;
			}
		}
		if (pix_on) {
			line({start_x, y}, {end_x + 1, y}, _draw_argb, fb);
		}
	}
}

void polygon::drawing::present()
{
	_gfx->present();
}

graphics::addr_t polygon::drawing::offset(const graphics::point& pos, const frame_buffer& fb) const
{
	return reinterpret_cast<graphics::addr_t>(fb.pixels + pos.x + _vp_min_pos.x + (pos.y + _vp_min_pos.y) * (fb.pitch_bytes / sizeof(graphics::color_t)));
}

bool polygon::drawing::is_in_bounds(const graphics::point& min_pos, const graphics::point& max_pos) const
{
	if (((min_pos.x < _vp_min_pos.x) && (max_pos.x > _vp_max_pos.x)) ||
	    ((min_pos.y < _vp_min_pos.y) && (max_pos.y > _vp_max_pos.y))) {
		return false;
	}

	return true;
}

void polygon::drawing::set_min_max()
{
	_min_pos = _gfx->get_min_position();
	_vp_min_pos.x = _min_pos.x;
	_vp_min_pos.y = _min_pos.y;

	_max_pos = _gfx->get_max_position();
	_vp_max_pos.x = _max_pos.x;
	_vp_max_pos.y = _max_pos.y;

	_vp_mid_pos.x = _vp_max_pos.x / 2;
	_vp_mid_pos.y = _vp_max_pos.y / 2;
}

void polygon::drawing::adjust_min(graphics::point& pos) const
{
	pos.x = max(pos.x, _vp_min_pos.x);
	pos.y = max(pos.y, _vp_min_pos.y);
}

void polygon::drawing::adjust_max(graphics::point& pos) const
{
	pos.x = min(pos.x, _vp_max_pos.x);
	pos.y = min(pos.y, _vp_max_pos.y);
}

void polygon::drawing::moveto(const graphics::point& pos, const frame_buffer& fb)
{
	_xy_pos = pos;
	_xy_addr = offset(pos, fb);
}

void polygon::drawing::lineto(const graphics::point& pos, const ARGB& argb)
{
	int dx, dy, loopx, loopy, tempx, tempy, signx, signy, incy;

	/* find the sign of dx & dy */

	dx = (int)pos.x - (int)_xy_pos.x;
	dy = (int)pos.y - (int)_xy_pos.y;

	if (dx > 0) {
		signx = 1;
	}
	else if (dx < 0) {
		signx = -1;
	}
	else {
		signx = 0;
	}

	if (dy > 0) {
		signy = 1;
		incy = _max_pos.x;
	}
	else if (dy < 0) {
		signy = -1;
		incy = -_max_pos.x;
	}
	else {
		signy = 0;
		incy = 0;
	}

	loopx = tempx = dx = std::abs(dx);
	loopy = tempy = dy = std::abs(dy);

	if (dx > dy) {
		while (loopx--) {
			if (_gfx->is_in_bounds(_xy_pos)) {
				putdirect(argb);
			}

			_xy_pos.x += signx;
			_xy_addr += signx;
			if ((tempx -= dy) <= 0) {
				_xy_pos.y += signy;
				_xy_addr += incy;
				tempx += dx;
			}
		}
	}
	else {
		while (loopy--) {
			if (_gfx->is_in_bounds(_xy_pos)) {
				putdirect(argb);
			}
			_xy_pos.y += signy;
			_xy_addr += incy;
			if ((tempy -= dx) <= 0) {
				_xy_pos.x += signx;
				_xy_addr += signx;
				tempy += dy;
			}
		}
	}
}

void polygon::drawing::line(const graphics::point& start, const graphics::point& end, const ARGB& argb, frame_buffer& fb)
{
	moveto(start, fb);
	lineto(end, argb);
}

void polygon::drawing::rect(const graphics::point& tl, const graphics::point& br, const ARGB& argb, frame_buffer& fb)
{
	for (uint32_t y = tl.y; y < br.y; y++) {
		line({tl.x, y}, {br.x, y}, argb, fb);
	}
}

ARGB polygon::drawing::getpixel(const graphics::point& pos, const frame_buffer& fb) const
{
	return *(offset(pos, fb));
}

void polygon::drawing::putpixel(const graphics::point& pos, ARGB& argb, frame_buffer& fb)
{
	*(offset(pos, fb)) = argb;
}

/* end of class polygon::drawing */

polygon::polygon(graphics& gfx)
{
	_gfx_ctx = new drawing(gfx);
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
				int color = atoi(line);
				_gfx_ctx->set_color(color);
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
	DBG(STR("        fill:", 1));
	_fill.print();
	DBG(STR("        normal:", 1));
	_normal.print();
	DBG(STR("        draw color: ", 1) << HEX(_gfx_ctx->get_color(), 8));
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
			_depth = fill.get(Z_);
			_gfx_ctx->make_color(abs(light_angle), m_gen);
			_draw_list.push_back(this);
		}
	}
}

static bool compare_depth(polygon* p1, polygon* p2)
{
	long depth1 = (long)p1->get_depth();
	long depth2 = (long)p2->get_depth();
	bool ret = (depth1 < depth2);
	// DBG(STR("depth1: ", 1) << depth1 << STR(" depth2: ", 1) << depth2 << STR(" ret: ", 1) << ret);
	return ret;
}

void polygon::sort()
{
	if (_draw_list.size() > 1) {
		_draw_list.sort(compare_depth);
	}
}

frame_buffer polygon::gfx_clear()
{
	return _gfx_ctx->clear();
}

void polygon::gfx_draw(frame_buffer& fb)
{
	_gfx_ctx->clear_scratch_pad();

	for (const auto v : _points) {
		_gfx_ctx->prepare(*v);
	}

	_gfx_ctx->draw(fb);
}

void polygon::gfx_present()
{
	_gfx_ctx->present();
}

void polygon::show_all()
{
	if (_draw_list.empty()) {
		DBG("show_all: empty");
		return;
	}

	frame_buffer fb = gfx_clear();

	while (_draw_list.size()) {
		polygon* poly = _draw_list.front();
		_draw_list.pop_front();
		poly->gfx_draw(fb);
	}

	gfx_present();
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

void polygon::sort_polygons()
{
	if (_draw_list.size()) {
		polygon* poly = _draw_list.front();
		poly->sort();
	}
}

void polygon::show_polygons()
{
	if (_draw_list.size()) {
		polygon* poly = _draw_list.front();
		poly->show_all();
	}
}

} // namespace polygon_ns
