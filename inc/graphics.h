#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "common.h"

namespace graphics_ns
{

using std::string;
using std::vector;

constexpr uint32_t MAX_X = 320;
constexpr uint32_t MAX_Y = 200;

struct graphics_error : std::runtime_error
{
	using std::runtime_error::runtime_error;
};

class graphics
{
public:

	struct frame_buffer
	{
		uint32_t* pixels {nullptr}; // ARGB8888
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

	typedef uint32_t color;

	union ARGB
	{
		color c {0};
		struct
		{
			color b : 8;
			color g : 8;
			color r : 8;
			color a : 8;
		} channels;
	};

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
	void clear_backbuffer(ARGB);
	void present(); /* uploads backbuffer, presents, swaps buffers */

private:

	string _title;
	uint32_t _w {0};
	uint32_t _h {0};
	uint32_t _scale {1};
	input_state _last_input {};

	// CPU-side double buffers
	vector<color> _buf_a;
	vector<color> _buf_b;
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
};

} // namespace graphics_ns
