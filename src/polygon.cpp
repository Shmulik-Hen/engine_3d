#include <algorithm>
#include <cmath>

// #define DEBUG_PRINTS
#include "polygon.h"
#include "utils.h"

namespace polygon_ns
{

using namespace graphics_ns;
using namespace matrix_ns;
using namespace vector_3_ns;
using addr_t = graphics::addr_t;
using ARGB = graphics::ARGB;
using frame_buffer = graphics::frame_buffer;
using color_idx = graphics::color_idx;
using point = graphics::point;
using color_t = graphics_ns::color_t;

polygon::drawing::drawing(const point& min_p, const point& max_p)
{
	set_min_max(min_p, max_p);
}

addr_t polygon::drawing::offset(frame_buffer& fb, const point& pos) const
{
	// clang-format off
	return reinterpret_cast<addr_t>(fb.pixels +
					_vp_min_pos.x + pos.x +
					(_vp_min_pos.y + pos.y) *
					(fb.pitch_bytes / sizeof(color_t)));
	// clang-format on
}

void polygon::drawing::moveto(frame_buffer& fb, const point& pos)
{
	_xy_pos = pos;
	_xy_addr = offset(fb, pos);
}

void polygon::drawing::set_min_max(const graphics_ns::graphics::point& min_p, const graphics_ns::graphics::point& max_p)
{
	_min_pos = min_p;
	_max_pos = max_p;

	_vp_min_pos = _min_pos;
	_vp_max_pos = _max_pos;

	_vp_mid_pos.x = (_vp_max_pos.x - _vp_min_pos.x) / 2;
	_vp_mid_pos.y = (_vp_max_pos.y - _vp_min_pos.y) / 2;
}

void polygon::drawing::adjust_min(point& pos)
{
	_bbox_tl.x = std::max(pos.x, _vp_min_pos.x);
	_bbox_tl.y = std::max(pos.y, _vp_min_pos.y);
}

void polygon::drawing::adjust_max(point& pos)
{
	_bbox_br.x = std::min(pos.x, _vp_max_pos.x);
	_bbox_br.y = std::min(pos.y, _vp_max_pos.y);
}

void polygon::drawing::create_bbox()
{
	point prev_pos, curr_pos, min_pos, max_pos;

	prev_pos = min_pos = max_pos = _scratch_pad[0];

	// update the bounding box
	for (int i = 1; i < (int)_scratch_pad.size(); i++) {
		curr_pos = _scratch_pad[i];
		min_pos.x = std::min(prev_pos.x, curr_pos.x);
		min_pos.y = std::min(min_pos.y, curr_pos.y);
		max_pos.x = std::max(max_pos.x, curr_pos.x);
		max_pos.y = std::max(max_pos.y, curr_pos.y);
		prev_pos = curr_pos;
	}

	adjust_min(min_pos);
	adjust_max(max_pos);
}

void polygon::drawing::lineto(const point& pos)
{
	int dx, loopx, tempx, signx;
	int dy, loopy, tempy, signy, incy;
	int s_off;

	/* find the sign of dx & dy */
	dx = pos.x - _xy_pos.x;
	dy = pos.y - _xy_pos.y;

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

	loopx = tempx = dx = std::min(std::abs(dx), _bbox_br.x);
	loopy = tempy = dy = std::min(std::abs(dy), _bbox_br.y);

	if (dx > dy) {
		while (loopx--) {
			_xy_pos.x += signx;
			_xy_addr += signx;
			if ((tempx -= dy) <= 0) {
				_xy_pos.y += signy;
				_xy_addr += incy;
				tempx += dx;
			}

			s_off = _xy_pos.y - _bbox_tl.y;
			auto& s = _spans[s_off];
			s.x_left = std::min(s.x_left, _xy_pos.x);
			s.x_right = std::max(s.x_right, _xy_pos.x);
		}
	}
	else {
		while (loopy--) {
			_xy_pos.y += signy;
			_xy_addr += incy;
			if ((tempy -= dx) <= 0) {
				_xy_pos.x += signx;
				_xy_addr += signx;
				tempy += dy;
			}

			s_off = _xy_pos.y - _bbox_tl.y;
			auto& s = _spans[s_off];
			s.x_left = std::min(s.x_left, _xy_pos.x);
			s.x_right = std::max(s.x_right, _xy_pos.x);
		}
	}
}

void polygon::drawing::bar(val_t left, val_t right)
{
	for (val_t i = left; i <= right; i++) {
		putdirect(_draw_color);
		_xy_pos.x++;
		_xy_addr++;
	}
}

void polygon::drawing::line(frame_buffer& fb, const point& start, const point& end)
{
	moveto(fb, start);
	lineto(end);
}

void polygon::drawing::rect(frame_buffer& fb, const point& tl, const point& br)
{
	for (int y = tl.y; y <= br.y; y++) {
		line(fb, {tl.x, y}, {br.x, y});
	}
}

ARGB polygon::drawing::getpixel(frame_buffer& fb, const point& pos)
{
	moveto(fb, pos);
	return *(offset(fb, pos));
}

void polygon::drawing::putpixel(frame_buffer& fb, const point& pos, ARGB& color)
{
	moveto(fb, pos);
	*(offset(fb, pos)) = color;
}

void polygon::drawing::set_color(const graphics_ns::graphics* gfx, int idx)
{
	_base_color = gfx->get_color_val(static_cast<color_idx>(idx));
}

void polygon::drawing::make_color(unit u)
{
#ifdef DEBUG_POLYGON
	unit factor = UNIT;
#else
	u = std::min(u, UNIT);
	unit factor = u / UNIT;
#endif
	_draw_color.c = _base_color.c;

	_draw_color.channel.r = (uint8_t)std::lroundf((unit)_base_color.channel.r * factor);
	_draw_color.channel.g = (uint8_t)std::lroundf((unit)_base_color.channel.g * factor);
	_draw_color.channel.b = (uint8_t)std::lroundf((unit)_base_color.channel.b * factor);
}

void polygon::drawing::clear_scratch_pad()
{
	_scratch_pad.clear();
}

void polygon::drawing::project(const vector_3& original, const vector_3& cam_position)
{
	vector_3 transformed = _trans_mat * original;

	if (transformed.get(Z_) < cam_position.get(Z_)) {
		_invalid = true;
	}

	vector_3 projected = vector_3::project(transformed, cam_position);
	val_t x = std::lroundf((unit)_vp_mid_pos.x + projected.get(X_));
	val_t y = std::lroundf((unit)_vp_mid_pos.y - projected.get(Y_));

	_scratch_pad.push_back({x, y});
}

void polygon::drawing::plot(frame_buffer& fb)
{
	point prev_pos, curr_pos, orig_pos;

	size_t size = _bbox_br.y - _bbox_tl.y + 1;
	_spans.resize(size);

	span s {_max_pos.x - 1, _min_pos.x + 1};
	for (int i = 0; i < (int)size; i++) {
		_spans[i] = s;
	}

	prev_pos = orig_pos = _scratch_pad[0];
	for (int i = 1; i < (int)_scratch_pad.size(); i++) {
		curr_pos = _scratch_pad[i];
		line(fb, prev_pos, curr_pos);
		prev_pos = curr_pos;
	}

	// close the shape
	lineto(orig_pos);
}

void polygon::drawing::fill(frame_buffer& fb)
{
	int y = _bbox_tl.y;
	for (int i = 0; i < (int)_spans.size(); i++) {
#ifdef DEBUG_POLYGON
		putpixel(fb, {_spans[i].x_left, y}, _debug_color);
		putpixel(fb, {_spans[i].x_right, y}, _debug_color);
#else
		moveto(fb, {_spans[i].x_left, y});
		bar(_spans[i].x_left, _spans[i].x_right);
#endif
		y++;
	}
}

void polygon::drawing::draw(frame_buffer& fb)
{
#ifndef DEBUG_POLYGON
	if (_invalid || _scratch_pad.size() < 3) {
		_invalid = false;
		_scratch_pad.clear();
		return;
	}
#endif

	create_bbox();
	plot(fb);
	fill(fb);

#ifdef DEBUG_POLYGON
	val_t fx = _fill_point.x;
	val_t fy = _fill_point.y;
	val_t nx = _normal_point.x;
	val_t ny = _normal_point.y;
	// draw the bbox
	_draw_color = _bbox_color;
	moveto(fb, _bbox_tl);
	lineto({_bbox_br.x, _bbox_tl.y});
	lineto({_bbox_br.x, _bbox_br.y});
	lineto({_bbox_tl.x, _bbox_br.y});
	lineto({_bbox_tl.x, _bbox_tl.y});
	// draw the filling point
	putpixel(fb, {fx - 1, fy}, _fill_color);
	putpixel(fb, {fx, fy}, _fill_color);
	putpixel(fb, {fx + 1, fy}, _fill_color);
	putpixel(fb, {fx, fy - 1}, _fill_color);
	putpixel(fb, {fx, fy}, _fill_color);
	putpixel(fb, {fx, fy + 1}, _fill_color);
	// draw the normal
	_draw_color = _normal_color;
	line(fb, {fx, fy}, {nx, ny});
	putdirect(_fill_color);
#endif
}

// ---------------------- end of class polygon::drawing -------------------------

polygon::polygon(const point& min_p, const point& max_p)
{
	_draw_ctx = new drawing(min_p, max_p);
}

polygon::~polygon()
{
	if (_draw_ctx) {
		delete _draw_ctx;
		_draw_ctx = nullptr;
	}

	for (auto* v : _points) {
		delete v;
	}

	_points.clear();
}

bool polygon::read(const graphics_ns::graphics* gfx, std::ifstream& ifs)
{
	LINE line;
	vector_3* v;
	int len;
	bool rc, finish = false;

	while (!ifs.eof() && !finish) {
		while ((!read_word(ifs, line)) && (!ifs.eof()));
		if (ifs.eof()) {
			break;
		}

		switch (line[1]) {
		case 'n':
			len = read_word(ifs, line);
			if (!len) {
				sys_error("polygon::read error polygon");
			}

			_name = line;
			break;
		case 'c':
			len = read_word(ifs, line);
			if (len) {
				int color = atoi(line);
				_draw_ctx->set_color(gfx, color);
			}
			else {
				sys_error("polygon::read error polygon");
			}
			break;
		case 'f':
			len = read_word(ifs, line);
			if (len) {
				_force = atoi(line);
			}
			else {
				sys_error("polygon::read error polygon");
			}
			break;
		case 'o': // TODO: FIX
			_normal.read(ifs);
			break;
		case 'v':
			v = new vector_3;
			rc = v->read(ifs);
			if (!rc) {
				delete v;
				sys_error("polygon::read error polygon");
			}
			_points.push_back(v);
			break;
		default:
			finish = true;
			ifs.seekg(-4, std::ios::cur);
			break;
		}
	}

	if (!verify()) {
		ERR("polygon: invalid polygon");
		return false;
	}

	_fill = find_fill();
	// _normal = find_normal(); // TODO: FIX

#ifdef DEBUG_POLYGON
	_debug_color = gfx->get_color_val(color_idx::white);
	_bbox_color = gfx->get_color_val(color_idx::green);
	_normal_color = gfx->get_color_val(color_idx::magenta);
	_fill_color = gfx->get_color_val(color_idx::purple);
#endif

	return true;
}

void polygon::print() const
{
#ifdef DEBUG_PRINTS
	DBG("      polygon:");
	DBG(STR("        name: ", 1) << _name);
	DBG(STR("        force: ", 1) << DEC(_force, 4));
	DBG(STR("        fill:", 1));
	_fill.print();
	DBG(STR("        normal:", 1));
	_normal.print();
	DBG(STR("        draw color: ", 1) << HEX(_draw_ctx->get_color(), 8));
	DBG(STR("        depth: ", 1) << _depth);
	if (!_points.empty()) {
		DBG("        points:");
		for (const auto vec : _points) {
			DBG(STR("        point:", 1));
			vec->print();
		}
	}
#endif // DEBUG_PRINTS
}

void polygon::update(matrix& m_trans, matrix& m_rot, frame_context& frame_ctx)
{
	vector_3 dist, fill, normal;
	unit view_angle, light_angle;

	_draw_ctx->_trans_mat = m_trans;
	_draw_ctx->_rot_mat = m_rot;

	fill = m_trans * _fill;
	normal = m_rot * _normal;

	// Convention: V is camera → surface
	// normals face camera when dot(N,V) < 0
	vector_3 V = vector_3::normalize(fill - frame_ctx.state->camera.position);
	view_angle = vector_3::dot(normal, V);

	if ((view_angle < EPSILON) || _force) {
		vector_3 L;

		if (frame_ctx.state->light.type == scene_ns::light_type::directional) {
			// Convention: light.direction is surface->light
			L = vector_3::normalize(frame_ctx.state->light.direction);
		}
		else {
			// Point light: surface->light
			L = vector_3::normalize(frame_ctx.state->light.position - fill);
		}

		light_angle = vector_3::dot(normal, L);
		if ((light_angle > EPSILON) || _force) {
			_depth = fill.get(Z_);
			_draw_ctx->make_color(std::abs(light_angle));
			frame_ctx.draw_vec->push_back(this);
		}
	}

#ifdef DEBUG_POLYGON
	static bool once = true;
	if (once) {
		once = false;
		_draw_ctx->_debug_normal = normal * 50.0f;
		_draw_ctx->_debug_fill = fill;
	}
#endif
}

void polygon::sort(frame_context& frame_ctx)
{
	if (frame_ctx.draw_vec->size() > 1) {
		// clang-format off
		std::sort(frame_ctx.draw_vec->begin(), frame_ctx.draw_vec->end(),
			  [](polygon* a, polygon* b) {
				return a->get_depth() < b->get_depth();
			  });
		// clang-format on
	}
}

bool polygon::is_consec()
{
	// TODO: implement
	return true;
}

bool polygon::is_degenerate()
{
	// TODO: implement
	return false;
}

bool polygon::is_planar()
{
	// TODO: implement
	return true;
}

bool polygon::verify()
{
	if (!_name.length()) {
		ERR("polygon: no name");
		return false;
	}

	if (_points.size() < 3) {
		ERR("polygon: need at least 3 vectors " << DEC(_points.size(), 4));
		return false;
	}

	if (!is_consec()) {
		ERR("polygon: non-consecutive");
		return false;
	}

	if (_points.size() > 3) {
		if (is_degenerate()) {
			ERR("polygon: degenrated");
			return false;
		}

		if (!is_planar()) {
			ERR("polygon: not planar");
			return false;
		}
	}

	return true;
}

void polygon::draw(frame_context& frame_ctx)
{
	_draw_ctx->clear_scratch_pad();

	for (const auto v : _points) {
		_draw_ctx->project(*v, frame_ctx.state->camera.position);
	}

#ifdef DEBUG_POLYGON
	static bool once = true;
	if (once) {
		once = false;
		vector_3 transformed, projected;
		val_t x, y;

		transformed = _draw_ctx->_trans_mat * _draw_ctx->_debug_normal;
		projected = vector_3::project(transformed, frame_ctx.state->camera.position);
		x = std::lroundf((unit)_draw_ctx->_vp_mid_pos.x + projected.get(X_));
		y = std::lroundf((unit)_draw_ctx->_vp_mid_pos.y - projected.get(Y_));
		_draw_ctx->_normal_point = {x, y};

		projected = vector_3::project(_draw_ctx->_debug_fill, frame_ctx.state->camera.position);
		x = std::lroundf((unit)_draw_ctx->_vp_mid_pos.x + projected.get(X_));
		y = std::lroundf((unit)_draw_ctx->_vp_mid_pos.y - projected.get(Y_));
		_draw_ctx->_fill_point = {x, y};
	}
#endif

	_draw_ctx->draw(frame_ctx.state->grfx.fb);
}

void polygon::draw_all(frame_context& frame_ctx)
{
	if (frame_ctx.draw_vec->empty()) {
		DBG("draw_all: empty");
		return;
	}

	for (polygon* poly : *frame_ctx.draw_vec) {
		if (!poly) {
			continue;
		}
		poly->draw(frame_ctx);
	}
}

vector_3 polygon::find_fill()
{
	vector_3 v;
	unit n = static_cast<unit>(_points.size());

	if (n) {
		for (const auto vec : _points) {
			v += *vec;
		}

		v.set(X_, v.get(X_) / n);
		v.set(Y_, v.get(Y_) / n);
		v.set(Z_, v.get(Z_) / n);
	}

	return v;
}

// TODO: wrong implementation - FIX
vector_3 polygon::find_normal()
{
	vector_3 *v1, *v2, v;

	if (_points.size() > 2) {
		v1 = _points[0];
		v2 = _points[1];

		v.set(X_, (v1->get(Y_) * v2->get(Z_) - v1->get(Z_) * v2->get(Y_)));
		v.set(Y_, (v1->get(Z_) * v2->get(X_) - v1->get(X_) * v2->get(Z_)));
		v.set(Z_, (v1->get(X_) * v2->get(Y_) - v1->get(Y_) * v2->get(X_)));

		v = vector_3::normalize(v);
	}

	return v;
}

void polygon::sort_polygons(frame_context& frame_ctx)
{
	sort(frame_ctx);
}

void polygon::draw_polygons(frame_context& frame_ctx)
{
	draw_all(frame_ctx);
}

} // namespace polygon_ns
