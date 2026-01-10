#pragma once

#include <cstdint>
#include <map>
#include <stdexcept>
#include <vector>

#include "common.h"

namespace graphics_ns
{

#define MAKE_COLOR(a, r, g, b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

using std::string;
using std::vector;

constexpr uint32_t MAX_X = 320;
constexpr uint32_t MAX_Y = 200;

class graphics
{
public:

	enum color_idx
	{
		__first_color__ = 0, // Dummy, not an actual color
		black = __first_color__,
		grey,
		silver,
		white,
		red,
		maroon,
		// orange,
		yellow,
		olive,
		lime,
		green,
		aqua,
		teal,
		blue,
		navy,
		magenta,
		purple,
		__last_color__ // Dummy, not an actual color
	};

	enum alpha_idx
	{
		__first_alpha__ = 0, // Dummy, not an actual alpha
		A1 = __first_alpha__,
		A2,
		A3,
		A4,
		A5,
		A6,
		A7,
		A8,
		A9,
		A10,
		A11,
		A12,
		A13,
		A14,
		A15,
		A16,
		__last_alpha__ // Dummy, not an actual alpha
	};

	typedef uint32_t color_t;

	union ARGB
	{
		color_t c {0};
		struct _channel
		{
			uint32_t b : 8;
			uint32_t g : 8;
			uint32_t r : 8;
			uint32_t a : 8;
		} channel;

		bool operator==(const ARGB& other) { return c == other.c; }
	};

	typedef ARGB* addr_t;

	struct color_data
	{
		string name;
		ARGB argb;
	};

	struct frame_buffer
	{
		color_t* pixels {nullptr}; // ARGB8888
		uint32_t width {0};
		uint32_t height {0};
		uint32_t pitch_bytes {0}; // bytes per row (typically width * 4)
	};

	struct input_state
	{
		bool quit {false};
		bool key_escape {false};
		// Extend later: arrows, mouse, etc.
	};

	struct point
	{
		uint32_t x = 0;
		uint32_t y = 0;
	};

	struct size
	{
		uint32_t w = 0;
		uint32_t h = 0;
	};

	typedef std::map<color_idx, color_data> color_map;
	typedef std::map<alpha_idx, uint8_t> alpha_map;

	graphics();
	graphics(const char*, uint32_t, uint32_t, uint32_t);
	~graphics();

	// class with allocated resource should not be copied
	graphics(const graphics&) = delete;
	graphics& operator=(const graphics&) = delete;

	/* event + timing */
	void poll_events(input_state&);
	std::uint64_t now_ticks() const;
	std::uint64_t tick_freq() const;

	/* framebuffer flow (engine-driven) */
	frame_buffer get_backbuffer();
	void fill_buffer(frame_buffer&, const ARGB&);
	frame_buffer get_clear_backbuffer(const ARGB&);
	void present(); /* uploads backbuffer, presents, swaps buffers */

	// color management
	ARGB get_color_val(const color_idx) const;
	string get_color_name(const color_idx) const;
	uint8_t get_alpha_val(const alpha_idx);
	void set_alpha(ARGB&, const uint8_t);
	uint32_t get_num_colors() const { return __last_color__; }
	uint32_t get_num_alphas() const { return __last_alpha__; }
	point get_min_position() const { return point {0, 0}; }
	point get_max_position() const { return point {_w, _h}; }
	bool is_in_bounds(point p) const { return !(p.x >= _w || p.y >= _h); }

private:

	string _title;
	uint32_t _w {0};
	uint32_t _h {0};
	uint32_t _scale {1};
	input_state _last_input {};
	color_map _colors;
	alpha_map _alphas;

	// CPU-side double buffers
	vector<color_t> _buf_a;
	vector<color_t> _buf_b;
	bool _a_is_front {true};

	// SDL objects (opaque pointers; stored as void* in header to avoid SDL includes here)
	void* _window {nullptr};
	void* _renderer {nullptr};
	void* _texture {nullptr};

	// Timing
	uint64_t _perf_freq {0};

	void init_graphics();
	void close_graphics();
	void swap_buffers();
	bool is_valid_color(color_idx i) const
	{
		return (i >= __first_color__ && i < __last_color__);
	}
	bool is_valid_alpha(alpha_idx a) const
	{
		return (a >= __first_alpha__ && a < __last_alpha__);
	}
};

} // namespace graphics_ns
