#include <algorithm>
#include <SDL2/SDL.h>

// #define DEBUG_PRINTS
#include "graphics.h"
#include "utils.h"

namespace graphics_ns
{

using std::min;
using std::string;
using ARGB = graphics::ARGB;           // bring nested type into namespace scope
using color_idx = graphics::color_idx; // same for color_idx
using color_map = graphics::color_map; // same for color_map
using input_state = graphics::input_state;

constexpr uint32_t DEFAULT_WIDTH = MAX_X;
constexpr uint32_t DEFAULT_HEIGHT = MAX_Y;
constexpr uint32_t DEFAULT_SCALE = 1;
constexpr const char* DEFAULT_NAME = "Graphics_SDL2";

graphics::graphics() :
	_title {DEFAULT_NAME},
	_w {DEFAULT_WIDTH},
	_h {DEFAULT_HEIGHT},
	_scale {DEFAULT_SCALE}
{
	init_graphics();
}

graphics::graphics(const char* title, uint32_t width, uint32_t height, uint32_t scale) :
	_title {title},
	_w {min(width, MAX_X)},
	_h {min(height, MAX_Y)},
	_scale {scale}
{
	if (!_w || !_h || !_scale || _title.empty()) {
		sys_error("Invalid width/height/scale/title");
	}

	init_graphics();
}

graphics::~graphics()
{
	close_graphics();
}

void graphics::poll_events(input_state& io)
{
	SDL_Event e;

	DBG("poll_events begin");
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			DBG("SDL_QUIT");
			io.quit = true;
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				DBG("SDLK_ESCAPE");
				io.key_escape = true;
				io.quit = true;
			}
			break;
		default:
			DBG("default: event type: " << HEX(e.type, 8));
			break;
		}
	}

	_last_input = io;
	DBG("poll_events end");
}

uint64_t graphics::now_ticks() const
{
	return static_cast<uint64_t>(SDL_GetPerformanceCounter());
}

uint64_t graphics::tick_freq() const
{
	return _perf_freq;
}

graphics::frame_buffer graphics::get_backbuffer()
{
	auto& back = _a_is_front ? _buf_b : _buf_a;
	frame_buffer fb {back.data(), _w, _h, _w * static_cast<uint32_t>(sizeof(color_t))};

	return fb;
}

void graphics::fill_buffer(frame_buffer& fb, const ARGB& argb)
{
	std::fill_n(fb.pixels, static_cast<size_t>(fb.width) * static_cast<size_t>(fb.height), argb.c);
}

graphics::frame_buffer graphics::get_clear_backbuffer(const ARGB& argb)
{
	frame_buffer fb = get_backbuffer();
	fill_buffer(fb, argb);
	return fb;
}

void graphics::present()
{
	frame_buffer fb = get_backbuffer();
	auto* tex = reinterpret_cast<SDL_Texture*>(_texture);
	auto* ren = reinterpret_cast<SDL_Renderer*>(_renderer);
	int rc;

	/* upload pixels to GPU-managed texture */
	rc = SDL_UpdateTexture(tex, nullptr, fb.pixels, fb.pitch_bytes);
	if (rc) {
		sys_error("SDL_UpdateTexture failed");
	}

	// Render texture scaled to window
	SDL_RenderClear(ren);

	SDL_Rect dst {0, 0, static_cast<int>(_w * _scale), static_cast<int>(_h * _scale)};

	rc = SDL_RenderCopy(ren, tex, nullptr, &dst);
	if (rc) {
		sys_error("SDL_RenderCopy failed");
	}

	SDL_RenderPresent(ren);

	/* only swap AFTER we showed the completed backbuffer */
	swap_buffers();
}

ARGB graphics::get_color_val(const color_idx idx) const
{
	if (!is_valid_color(idx)) {
		DBG("get_color_val: invalid color_idx");
		return ARGB {};
	}

	auto it = _colors.find(idx);
	if (it != _colors.end()) {
		color_data d = it->second;
		return d.argb;
	}

	DBG("get_color_val: return fallback");
	return ARGB {}; // fallback for unknown idx
}

string graphics::get_color_name(const color_idx idx) const
{
	if (!is_valid_color(idx)) {
		return "";
	}

	auto it = _colors.find(idx);
	if (it != _colors.end()) {
		color_data d = it->second;
		return d.name;
	}

	return "";
}

uint8_t graphics::get_alpha_val(const alpha_idx idx)
{
	if (!is_valid_alpha(idx)) {
		DBG("get_alpha_val: invalid alpha_idx: " << DEC((int)idx, 2));
		return 0;
	}

	auto it = _alphas.find(idx);
	if (it != _alphas.end()) {
		DBG("get_alpha_val: " << HEX((int)it->second, 2));
		return it->second;
	}

	DBG("get_alpha_val: returning default");
	return 0;
}

void graphics::set_alpha(ARGB& argb, const uint8_t alpha)
{
	argb.channel.a = alpha;
}

void graphics::init_graphics()
{
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Texture* tex;
	int rc;

	// clang-format off
	const char* tmp_col[__last_color__] = {
		"black",	// 0
		"grey",		// 1
		"silver",	// 2
		"white",	// 3
		"red",		// 4
		"maroon",	// 5
		// orange,
		"yellow",	// 6
		"olive",	// 7
		"lime",		// 8
		"green",	// 9
		"aqua",		// 10
		"teal",		// 11
		"blue",		// 12
		"navy",		// 13
		"magenta",	// 14
		"purple"	// 15
	};

	const uint8_t channels[__last_color__][4] = {
		{ 255,   0,   0,   0 }, // 0  black
		{ 255, 128, 128, 128 }, // 1  grey
		{ 255, 192, 192, 192 }, // 2  silver
		{ 255, 255, 255, 255 }, // 3  white
		{ 255, 255,   0,   0 }, // 4  red
		{ 255, 128,   0,   0 }, // 5  maroon
		// {255,   0,   0,   0}, // ?  orange
		{ 255, 255, 255,   0 }, // 6  yellow
		{ 255, 128, 128,   0 }, // 7  olive
		{ 255,   0, 255,   0 }, // 8  lime
		{ 255,   0, 128,   0 }, // 9  green
		{ 255,   0, 255, 255 }, // 10 aqua
		{ 255,   0, 128, 128 }, // 11 teal
		{ 255,   0,   0, 255 }, // 12 blue
		{ 255,   0,   0, 128 }, // 13 navy
		{ 255, 255,   0, 255 }, // 14 magenta
		{ 255, 128,   0, 128 }  // 15 purple
	};

	uint8_t alpha_vals[__last_alpha__] = {
		0x00,
		0x11,
		0x22,
		0x33,
		0x44,
		0x55,
		0x66,
		0x77,
		0x88,
		0x99,
		0xaa,
		0xbb,
		0xcc,
		0xdd,
		0xee,
		0xff
	};
	// clang-format on

	_colors.clear();
	for (uint32_t i = __first_color__; i < __last_color__; i++) {
		color_idx idx = static_cast<color_idx>(i);
		const char* s = tmp_col[i];
		ARGB argb;
		argb.c = MAKE_COLOR(channels[idx][0], channels[idx][1], channels[idx][2], channels[idx][3]);

		color_data d;
		d.name = string {s};
		d.argb.c = argb.c;

		std::pair p = {idx, d};
		_colors.insert(p);
	}

	if (_colors.empty()) {
		sys_error("colors init failed");
	}

	_alphas.clear();
	for (uint32_t i = __first_alpha__; i < __last_alpha__; i++) {
		alpha_idx idx = static_cast<alpha_idx>(i);
		std::pair p = {idx, alpha_vals[idx]};
		_alphas.insert(p);
	}

	if (_alphas.empty()) {
		sys_error("alpha init failed");
	}

	rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	if (rc) {
		sys_error("SDL_Init failed");
	}

	/* crisp integer scaling */
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	win = SDL_CreateWindow(_title.c_str(),
	                       SDL_WINDOWPOS_CENTERED,
	                       SDL_WINDOWPOS_CENTERED,
	                       _w * _scale,
	                       _h * _scale,
	                       SDL_WINDOW_SHOWN);
	if (!win) {
		sys_error("SDL_CreateWindow failed");
	}
	_window = win;

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (!ren) {
		sys_error("SDL_CreateRenderer failed");
	}
	_renderer = ren;

	// Streaming texture so we can upload ARGB pixels each frame
	tex = SDL_CreateTexture(ren,
	                        SDL_PIXELFORMAT_ARGB8888,
	                        SDL_TEXTUREACCESS_STREAMING,
	                        _w,
	                        _h);
	if (!tex) {
		sys_error("SDL_CreateTexture failed");
	}
	_texture = tex;

	// CPU buffers
	_buf_a.resize(static_cast<size_t>(_w) * static_cast<size_t>(_h));
	_buf_b.resize(static_cast<size_t>(_w) * static_cast<size_t>(_h));

	/* avoid uninitialized visuals */
	color_t c = MAKE_COLOR(alpha_vals[A16], channels[black][1], channels[black][2], channels[black][3]);
	std::fill(_buf_a.begin(), _buf_a.end(), c);
	std::fill(_buf_b.begin(), _buf_b.end(), c);

	_perf_freq = static_cast<uint64_t>(SDL_GetPerformanceFrequency());

	DBG("graphics:" << ENDL << STR("  final values:", 1) << ENDL
	                << STR("    buff size: ", 14) << DEC(_buf_a.size(), 4) << ENDL
	                << STR("    freq: ", 14) << DEC(_perf_freq, 8) << ENDL);
}

void graphics::close_graphics()
{
	// Destroy in reverse creation order
	if (_texture) {
		SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(_texture));
	}

	if (_renderer) {
		SDL_DestroyRenderer(reinterpret_cast<SDL_Renderer*>(_renderer));
	}

	if (_window) {
		SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(_window));
	}

	SDL_Quit();
}

void graphics::swap_buffers()
{
	_a_is_front = !_a_is_front;
}

} // namespace graphics_ns
