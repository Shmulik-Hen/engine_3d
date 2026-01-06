#include <algorithm>
#include <SDL2/SDL.h>

#include "graphics.h"

namespace graphics_ns
{

#if 0
uint32_t G_OFFSET = 0;
uint32_t G_POS_X = 0;
uint32_t G_POS_Y = 0;
uint32_t G_MIN_X = 0;
uint32_t G_MIN_Y = 0;
uint32_t G_MAX_X = 320;
uint32_t G_MAX_Y = 200;
uint32_t G_COLOR = 0;
uint32_t G_EDGE_COLOR = 0x00000000;
uint32_t G_BLACK = 0x00000000;
int vp_min_x;
int vp_min_y;
int vp_max_x;
int vp_max_y;
int vp_mid_x;
int vp_mid_y;
#define offset(x, y)      ((x) + G_MIN_X + ((y) + G_MIN_Y) * (G_MAX_X - G_MIN_X + 1))
#define putpixel(x, y, c) BUFFER[offset((x), (y))] = (c)
#define getpixel(x, y)    BUFFER[offset((x), (y))]
#define m_setcolor(c)     G_COLOR = (c)
#define getcolor()        G_COLOR
#define putdirect(o, c)   BUFFER[(o)] = (c)
#define getdirect(o)      BUFFER[(o)]
#define moveto(x, y)                 \
	G_OFFSET = offset((x), (y)); \
	G_POS_X = (x);               \
	G_POS_Y = (y)
#endif // 0

using std::max;
using std::min;
using std::string;

enum flag
{
	OFF,
	ON
};

constexpr uint32_t DEFAULT_WIDTH = MAX_X;
constexpr uint32_t DEFAULT_HEIGHT = MAX_Y;
constexpr uint32_t DEFAULT_SCALE = 1;
constexpr const char* DEFAULT_NAME = "Graphics_SDL2";

static graphics_error sdl_fail(const char* what)
{
	string msg = string(what) + ": " + SDL_GetError();
	return graphics_error(msg);
}

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
		throw graphics_error("Invalid width/height/scale/title");
	}

	init_graphics();
}

graphics::~graphics()
{
	close_graphics();
}

void graphics::init_graphics()
{
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Texture* tex;
	int rc;

	DBG("graphics:" << ENDL << STR("  initial values:", 1) << ENDL
	                << STR("    title:", 14) << _title << ENDL
	                << STR("    width:", 14) << DEC(_w, 4) << ENDL
	                << STR("    height:", 14) << DEC(_h, 4) << ENDL
	                << STR("    scale:", 14) << DEC(_scale, 4) << ENDL);

	rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	if (rc) {
		throw sdl_fail("SDL_Init failed");
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
		throw sdl_fail("SDL_CreateWindow failed");
	}
	_window = win;

	// SDL_RENDERER_ACCELERATED is typically fine. If you want absolute determinism, you can try SOFTWARE.
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (!ren) {
		throw sdl_fail("SDL_CreateRenderer failed");
	}
	_renderer = ren;

	// Streaming texture so we can upload ARGB pixels each frame
	tex = SDL_CreateTexture(ren,
	                        SDL_PIXELFORMAT_ARGB8888,
	                        SDL_TEXTUREACCESS_STREAMING,
	                        _w,
	                        _h);
	if (!tex) {
		throw sdl_fail("SDL_CreateTexture failed");
	}
	_texture = tex;

	// CPU buffers
	_buf_a.resize(static_cast<size_t>(_w) * static_cast<size_t>(_h));
	_buf_b.resize(static_cast<size_t>(_w) * static_cast<size_t>(_h));

	/* avoid uninitialized visuals */
	color c = 0xff000000;
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
	frame_buffer fb {back.data(), _w, _h, _w * static_cast<uint32_t>(sizeof(uint32_t))};

	return fb;
}

void graphics::clear_backbuffer(ARGB argb)
{
	auto fb = get_backbuffer();
	std::fill_n(fb.pixels, static_cast<size_t>(fb.width) * static_cast<size_t>(fb.height), argb.c);
}

void graphics::swap_buffers()
{
	_a_is_front = !_a_is_front;
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
		throw sdl_fail("SDL_UpdateTexture failed");
	}

	// Render texture scaled to window
	SDL_RenderClear(ren);

	SDL_Rect dst {0, 0, static_cast<int>(_w * _scale), static_cast<int>(_h * _scale)};

	rc = SDL_RenderCopy(ren, tex, nullptr, &dst);
	if (rc) {
		throw sdl_fail("SDL_RenderCopy failed");
	}

	SDL_RenderPresent(ren);

	/* only swap AFTER we showed the completed backbuffer */
	swap_buffers();
}

} // namespace graphics_ns
