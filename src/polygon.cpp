#include <cmath>

// #define DEBUG_PRINTS
#include "polygon.h"
#include "render_conventions.h"
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
using frame_context = scene_ns::frame_context;
using render_ns::front_facing;
using render_ns::lambert;
using vector_t = polygon_ns::polygon::vector_t;

addr_t polygon::drawing::offset(frame_context& frame_ctx, const point& pos) const
{
	// clang-format off
	return reinterpret_cast<addr_t>(frame_ctx.state->grfx.fb.pixels +
					frame_ctx.state->vp.min_pos.x + pos.x +
					(frame_ctx.state->vp.min_pos.y + pos.y) *
					(frame_ctx.state->grfx.fb.pitch_pixels));
	// clang-format on
}

void polygon::drawing::moveto(frame_context& frame_ctx, const point& pos)
{
	_xy_pos = pos;
	_xy_addr = offset(frame_ctx, pos);
}

void polygon::drawing::adjust_min(frame_context& frame_ctx, point& pos)
{
	_bbox_tl.x = std::max(pos.x, frame_ctx.state->vp.min_pos.x);
	_bbox_tl.y = std::max(pos.y, frame_ctx.state->vp.min_pos.y);
}

void polygon::drawing::adjust_max(frame_context& frame_ctx, point& pos)
{
	_bbox_br.x = std::min(pos.x, frame_ctx.state->vp.max_pos.x);
	_bbox_br.y = std::min(pos.y, frame_ctx.state->vp.max_pos.y);
}

void polygon::drawing::create_bbox(frame_context& frame_ctx)
{
	point prev_pos, curr_pos, min_pos, max_pos;

	prev_pos = min_pos = max_pos = _scratch_pad[0];

	// update the bounding box
	for (int i = 1; i < (int)_scratch_pad.size(); i++) {
		curr_pos = _scratch_pad[i];
		min_pos.x = std::min(min_pos.x, curr_pos.x);
		min_pos.y = std::min(min_pos.y, curr_pos.y);
		max_pos.x = std::max(max_pos.x, curr_pos.x);
		max_pos.y = std::max(max_pos.y, curr_pos.y);
		prev_pos = curr_pos;
	}

	adjust_min(frame_ctx, min_pos);
	adjust_max(frame_ctx, max_pos);
}

void polygon::drawing::lineto(frame_context& frame_ctx, const point& pos)
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
		incy = frame_ctx.state->grfx.fb.pitch_pixels;
	}
	else if (dy < 0) {
		signy = -1;
		incy = -frame_ctx.state->grfx.fb.pitch_pixels;
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

void polygon::drawing::stroke_to(frame_context& frame_ctx, const point& pos)
{
	int dx = pos.x - _xy_pos.x;
	int dy = pos.y - _xy_pos.y;

	int signx = (dx > 0) - (dx < 0);
	int signy = (dy > 0) - (dy < 0);

	int ax = std::abs(dx);
	int ay = std::abs(dy);

	int incy = signy * frame_ctx.state->grfx.fb.pitch_pixels;

	int err = ax - ay;

	while (true) {
		putdirect(_draw_color);

		if (_xy_pos.x == pos.x && _xy_pos.y == pos.y) {
			break;
		}

		int e2 = 2 * err;
		if (e2 > -ay) {
			err -= ay;
			_xy_pos.x += signx;
			_xy_addr += signx;
		}
		if (e2 < ax) {
			err += ax;
			_xy_pos.y += signy;
			_xy_addr += incy;
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

void polygon::drawing::line(frame_context& frame_ctx, const point& start, const point& end)
{
	moveto(frame_ctx, start);
	stroke_to(frame_ctx, end);
}

void polygon::drawing::rect(frame_context& frame_ctx, const point& tl, const point& br)
{
	for (int y = tl.y; y <= br.y; y++) {
		moveto(frame_ctx, {tl.x, y});
		bar(tl.x, br.x);
	}
}

ARGB polygon::drawing::getpixel(frame_context& frame_ctx, const point& pos)
{
	moveto(frame_ctx, pos);
	return *(offset(frame_ctx, pos));
}

void polygon::drawing::putpixel(frame_context& frame_ctx, const point& pos, ARGB& color)
{
	moveto(frame_ctx, pos);
	*(offset(frame_ctx, pos)) = color;
}

void polygon::drawing::set_color(const graphics_ns::graphics* gfx, int idx)
{
	_base_color = gfx->get_color_val(static_cast<color_idx>(idx));
}

void polygon::drawing::make_color(unit u)
{
	u = std::min(u, UNIT);
	unit factor = u / UNIT;

	_draw_color.c = _base_color.c;

	_draw_color.channel.r = (uint8_t)std::lroundf((unit)_base_color.channel.r * factor);
	_draw_color.channel.g = (uint8_t)std::lroundf((unit)_base_color.channel.g * factor);
	_draw_color.channel.b = (uint8_t)std::lroundf((unit)_base_color.channel.b * factor);
}

void polygon::drawing::clear_scratch_pad()
{
	_scratch_pad.clear();
}

void polygon::drawing::project(const vector_3& original, frame_context& frame_ctx, bool force)
{
	vector_3 transformed = _trans_mat * original;

	if (!force && (transformed.get(Z_) < frame_ctx.state->camera.position.get(Z_))) {
		_invalid = true;
	}

	vector_3 projected = vector_3::project(transformed,
	                                       frame_ctx.state->camera.position,
	                                       frame_ctx.state->proj.focal_len,
	                                       frame_ctx.state->proj.near_eps);
	val_t x = std::lroundf((unit)frame_ctx.state->vp.mid_pos.x + projected.get(X_));
	val_t y = std::lroundf((unit)frame_ctx.state->vp.mid_pos.y - projected.get(Y_));

	_scratch_pad.push_back({x, y});
}

void polygon::drawing::plot(frame_context& frame_ctx)
{
	point prev_pos, curr_pos, orig_pos;

	size_t size = _bbox_br.y - _bbox_tl.y + 1;
	_spans.resize(size);

	span s {frame_ctx.state->vp.max_pos.x - 1, frame_ctx.state->vp.min_pos.x + 1};
	for (int i = 0; i < (int)size; i++) {
		_spans[i] = s;
	}

	prev_pos = orig_pos = _scratch_pad[0];
	for (int i = 1; i < (int)_scratch_pad.size(); i++) {
		curr_pos = _scratch_pad[i];
		moveto(frame_ctx, prev_pos);
		lineto(frame_ctx, curr_pos);
		prev_pos = curr_pos;
	}

	// close the shape
	lineto(frame_ctx, orig_pos);
}

void polygon::drawing::fill(frame_context& frame_ctx)
{
	int y = _bbox_tl.y;
#ifdef DEBUG_POLYGON
	ARGB c = frame_ctx.state->grfx.gfx->get_color_val(_debug_color);
#endif

	for (int i = 0; i < (int)_spans.size(); i++) {
#ifdef DEBUG_POLYGON
		putpixel(frame_ctx, {_spans[i].x_left, y}, c);
		putpixel(frame_ctx, {_spans[i].x_right, y}, c);
#else
		moveto(frame_ctx, {_spans[i].x_left, y});
		bar(_spans[i].x_left, _spans[i].x_right);
#endif
		y++;
	}
}

void polygon::drawing::draw(frame_context& frame_ctx)
{
	if (_scratch_pad.size() < 3 || _invalid) {
		_invalid = false;
		_scratch_pad.clear();
		return;
	}

	create_bbox(frame_ctx);
	plot(frame_ctx);
	fill(frame_ctx);

#ifdef DEBUG_POLYGON
	val_t fx = _fill_point.x;
	val_t fy = _fill_point.y;
	val_t nx = _normal_point.x;
	val_t ny = _normal_point.y;
	// draw the filling point
	ARGB c = frame_ctx.state->grfx.gfx->get_color_val(_fill_color);
	putpixel(frame_ctx, {fx - 1, fy}, c);
	putpixel(frame_ctx, {fx, fy}, c);
	putpixel(frame_ctx, {fx + 1, fy}, c);
	putpixel(frame_ctx, {fx, fy - 1}, c);
	putpixel(frame_ctx, {fx, fy}, c);
	putpixel(frame_ctx, {fx, fy + 1}, c);
	// draw the bbox
	_draw_color = frame_ctx.state->grfx.gfx->get_color_val(_bbox_color);
	line(frame_ctx, _bbox_tl, {_bbox_br.x, _bbox_tl.y});
	line(frame_ctx, {_bbox_br.x, _bbox_tl.y}, _bbox_br);
	line(frame_ctx, _bbox_br, {_bbox_tl.x, _bbox_br.y});
	line(frame_ctx, {_bbox_tl.x, _bbox_br.y}, _bbox_tl);
	// draw the normal
	_draw_color = frame_ctx.state->grfx.gfx->get_color_val(_normal_color);
	line(frame_ctx, {fx, fy}, {nx, ny});
	putpixel(frame_ctx, {nx, ny}, c);
#endif
}

// ---------------------- end of class polygon::drawing -------------------------

polygon::polygon()
{
	_draw_ctx = new drawing();
}

polygon::~polygon()
{
	if (_draw_ctx) {
		delete _draw_ctx;
		_draw_ctx = nullptr;
	}

	_points.clear();
}

static unit len2(const vector_3& v)
{
	const unit x = v.get(X_);
	const unit y = v.get(Y_);
	const unit z = v.get(Z_);
	return x * x + y * y + z * z;
}

static unit dist2(const vector_3& a, const vector_3& b)
{
	const unit dx = a.get(X_) - b.get(X_);
	const unit dy = a.get(Y_) - b.get(Y_);
	const unit dz = a.get(Z_) - b.get(Z_);
	return dx * dx + dy * dy + dz * dz;
}

static unit max_radius_from(const vector_t& points, const vector_3& p0)
{
	unit max_d2 = ZERO;

	for (const auto& p : points) {
		const vector_3 d = p - p0;
		max_d2 = std::max(max_d2, len2(d));
	}

	return std::sqrt(max_d2);
}

bool polygon::is_consec()
{
	const std::size_t n = _points.size();

	// Choose a tolerance. Using EPSILON in world units can be too small or too big,
	// but this is a safe start.
	const unit eps2 = (EPSILON * CONSEC_TOLERANCE_SCALE) * (EPSILON * CONSEC_TOLERANCE_SCALE);

	// 1) No consecutive duplicates (including wraparound)
	for (std::size_t i = 0; i < n; ++i) {
		const vector_3& a = _points[i];
		const vector_3& b = _points[(i + 1) % n];
		if (dist2(a, b) <= eps2) {
			return false;
		}
	}

	// 2) No duplicates anywhere (recommended)
	for (std::size_t i = 0; i < n; ++i) {
		for (std::size_t j = i + 1; j < n; ++j) {
			if (dist2(_points[i], _points[j]) <= eps2) {
				return false;
			}
		}
	}

	return true;
}

bool polygon::is_degenerate()
{
	vector_3 n = find_normal(); // already handles n==3 vs Newell
	return (len2(n) <= EPSILON * EPSILON);
}

bool polygon::is_planar()
{
	const vector_3 p0 = _points[0];
	const vector_3 N = find_normal();

	// Scale tolerance to polygon size to avoid false negatives on large coordinates.
	// radius ~ "size" of polygon in world units.
	const unit radius = max_radius_from(_points, p0);
	const unit tol = std::max(EPSILON * 10, EPSILON * radius);

	for (const auto& p : _points) {
		const vector_3 d = p - p0;
		const unit dist = vector_3::dot(N, d); // signed distance scaled by |N| (|N| == 1)
		if (std::abs(dist) > tol) {
			return false;
		}
	}

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

	if (is_degenerate()) {
		ERR("polygon: degenerate");
		return false;
	}

	if (_points.size() > 3 && !is_planar()) {
		ERR("polygon: not planar");
		return false;
	}

	return true;
}

vector_3 polygon::find_fill()
{
	vector_3 v;
	unit n = static_cast<unit>(_points.size());

	if (n) {
		for (const auto& vec : _points) {
			v += vec;
		}

		v.set(X_, v.get(X_) / n);
		v.set(Y_, v.get(Y_) / n);
		v.set(Z_, v.get(Z_) / n);
	}

	return v;
}

vector_3 polygon::find_normal()
{
	const std::size_t n = _points.size();

	if (n == 3) {
		// --- Fast/accurate path for triangles ---
		const vector_3& p0 = _points[0];
		const vector_3& p1 = _points[1];
		const vector_3& p2 = _points[2];

		const vector_3 e1 = p1 - p0;
		const vector_3 e2 = p2 - p0;

		vector_3 cr = vector_3::cross(e1, e2);

		return vector_3::normalize(cr);
	}
	else {
		// --- Robust path for 4+ vertices: Newell's method ---
		vector_3 nn(ZERO, ZERO, ZERO);

		for (std::size_t i = 0; i < n; ++i) {
			const vector_3& cur = _points[i];
			const vector_3& nxt = _points[(i + 1) % n];

			const unit cx = cur.get(X_);
			const unit cy = cur.get(Y_);
			const unit cz = cur.get(Z_);

			const unit nx = nxt.get(X_);
			const unit ny = nxt.get(Y_);
			const unit nz = nxt.get(Z_);

			nn.set(X_, nn.get(X_) + (cy - ny) * (cz + nz));
			nn.set(Y_, nn.get(Y_) + (cz - nz) * (cx + nx));
			nn.set(Z_, nn.get(Z_) + (cx - nx) * (cy + ny));
		}

		return vector_3::normalize(nn);
	}
}

void polygon::init_from_def(frame_context& ctx, const config_ns::polygon_def& def)
{
	_name = def.name;
	_draw_ctx->set_color(ctx.state->grfx.gfx.get(), def.color_index);
#ifdef DEBUG_POLYGON
	_force = true;
#else
	_force = def.force;
#endif

	_points.clear();
	for (const auto& v : def.points) {
		_points.push_back(v);
	}

	if (!verify()) {
		sys_error("polygon::init_from_def: verify failed: ", _name.c_str());
	}

	// compute fill + normal from geometry
	_fill = find_fill();
	_normal = find_normal();

	// optional: if cfg normal exists, assert winding matches during transition
	if (def.normal_cfg.has_value()) {
		_config_normal = vector_3::normalize(def.normal_cfg.value());
		if (vector_3::dot(_normal, _config_normal) < ZERO) {
			// either flip or warn; I recommend warn+flip until old configs cleaned
			WARN("computed normal != configuration normal. flipping");
			_normal = _normal * -UNIT;
		}
	}
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
#ifdef DEBUG_POLYGON
				_force = true;
#else
				int f = atoi(line);
				_force = (f ? true : false);
#endif
			}
			else {
				sys_error("polygon::read error polygon");
			}
			break;
		case 'o': // TODO: FIX
			_config_normal.read(ifs);
			break;
		case 'v':
			v = new vector_3;
			rc = v->read(ifs);
			if (!rc) {
				delete v;
				sys_error("polygon::read error polygon");
			}
			_points.push_back(*v);
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
	_normal = find_normal();

	unit d = vector_3::dot(_normal, _config_normal);
	if (d < ZERO) {
		WARN("polygon::read: winding mismatch vs config normal" << _name);
		DBG("polygon::read: config normal:");
		_config_normal.print();
		DBG("polygon::read: calculated normal:");
		_normal.print();
	}

	return true;
}

void polygon::print() const
{
#ifdef DEBUG_PRINTS
	DBG("polygon:");
	DBG(STR("  name: ", 1) << _name);
	DBG(STR("  force: ", 1) << DEC(_force, 4));
	DBG(STR("  fill:", 1));
	_fill.print();
	DBG(STR("  normal:", 1));
	_normal.print();
	DBG(STR("  config normal:", 1));
	_config_normal.print();
	DBG(STR("  draw color: ", 1) << HEX(_draw_ctx->get_color(), 8));
	DBG(STR("  depth: ", 1) << _depth);
	if (!_points.empty()) {
		DBG("  points:");
		for (const auto& vec : _points) {
			DBG(STR("  point:", 1));
			vec.print();
		}
	}
#endif // DEBUG_PRINTS
}

void polygon::update(matrix& m_trans, matrix& m_rot, frame_context& frame_ctx)
{
	vector_3 dist, fill, normal, L;
	unit atten = UNIT;

	_draw_ctx->_trans_mat = m_trans;
	_draw_ctx->_rot_mat = m_rot;

	fill = m_trans * _fill;
	normal = m_rot * _normal;

	// Convention: V is camera → surface
	// normals face camera when dot(N,V) < 0
	vector_3 V = vector_3::normalize(fill - frame_ctx.state->camera.position);
	if (!front_facing(normal, V, _force)) {
		return;
	}

	if (frame_ctx.state->light.type == scene_ns::light_type::directional) {
		// Convention: light.direction is surface->light
		L = vector_3::normalize(frame_ctx.state->light.direction);
	}
	else {
		// Point light: surface->light
		vector_3 to_light = frame_ctx.state->light.position - fill; // surface -> light

		// Distance attenuation
		const unit r2 = vector_3::dot(to_light, to_light);
		// Tune k to your world scale; start here and adjust
		const unit k = DIST_ATTEN;
		atten = UNIT / (UNIT + k * r2);

		L = vector_3::normalize(to_light);
	}

	unit light_angle = lambert(normal, L);
	unit intensity = std::abs(light_angle) * atten;
	if ((intensity > EPSILON) || _force) {
		_depth = fill.get(Z_);
		_draw_ctx->make_color(std::abs(intensity));
		frame_ctx.draw_vec->push_back(this);
	}

#ifdef DEBUG_POLYGON
	_draw_ctx->_debug_fill = fill;
	_draw_ctx->_debug_normal = fill + normal * 50.0f;
#endif
}

void polygon::draw(frame_context& frame_ctx)
{
	_draw_ctx->clear_scratch_pad();

	for (const auto& v : _points) {
		_draw_ctx->project(v, frame_ctx, _force);
	}

#ifdef DEBUG_POLYGON
	vector_3 projected;
	val_t x, y;

	projected = vector_3::project(_draw_ctx->_debug_normal,
	                              frame_ctx.state->camera.position,
	                              frame_ctx.state->proj.focal_len,
	                              frame_ctx.state->proj.near_eps);
	x = std::lroundf((unit)frame_ctx.state->vp.vp_mid_pos.x + projected.get(X_));
	y = std::lroundf((unit)frame_ctx.state->vp.vp_mid_pos.y - projected.get(Y_));
	_draw_ctx->_normal_point = {x, y};

	projected = vector_3::project(_draw_ctx->_debug_fill,
	                              frame_ctx.state->camera.position,
	                              frame_ctx.state->proj.focal_len,
	                              frame_ctx.state->proj.near_eps);
	x = std::lroundf((unit)frame_ctx.state->vp.vp_mid_pos.x + projected.get(X_));
	y = std::lroundf((unit)frame_ctx.state->vp.vp_mid_pos.y - projected.get(Y_));
	_draw_ctx->_fill_point = {x, y};
#endif

	_draw_ctx->draw(frame_ctx);
}

} // namespace polygon_ns
