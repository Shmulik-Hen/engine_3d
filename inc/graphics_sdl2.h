#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <cstdint>
#include <functional>
#include <stdexcept>
#include <vector>
#include "common.h"
#include "graphics_base.h"

namespace graphics_ns
{

using color = graphics_base_ns::graphics_base::color;
using std::string;
using std::vector;

constexpr uint32_t MAX_X = 320;
constexpr uint32_t MAX_Y = 200;

struct graphics_error : std::runtime_error
{
	using std::runtime_error::runtime_error;
};

class graphics : private graphics_base_ns::graphics_base
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
		bool quit = false;
		bool key_escape = false;
		// Extend later: arrows, mouse, etc.
	};

	union rgba
	{
		color c;
		struct
		{
			color a : 8;
			color r : 8;
			color g : 8;
			color b : 8;
		} channels;
	};

	using update_fn = std::function<void(double dt_seconds, const input_state& input)>;
	using render_fn = std::function<void(frame_buffer& fb)>;

	graphics();
	graphics(const char*, uint32_t, uint32_t, uint32_t);
	~graphics();

	// class with allocated resource should not be copied
	graphics(const graphics&) = delete;
	graphics& operator=(const graphics&) = delete;

	// Runs a fixed-timestep loop. Render is called once per displayed frame.
	// update() is called zero or more times per frame depending on accumulator.
	void run(const update_fn& update_fn, const render_fn& render_fn);

	// Optional: allow clean stop from outside
	void request_quit() { _input.quit = true; }

	virtual void init_graphics() override;
	virtual void close_graphics() override;
	// virtual void set_viewport(const point&, const size&) override;
	// virtual void clear_viewport() override;
	// virtual void refresh() const override;
	// virtual bool is_in_bounds(point) const override;
	// virtual uint32_t get_width() const override { return _w; };
	// virtual uint32_t get_height() const override { return _h; };
	// virtual uint32_t get_buff_size() { return _buf_a.size(); }

	// virtual void init_palette();
	// virtual void get_palette();
	// virtual void set_palette();
	// virtual void set_color(const color&);
	// virtual color get_color(const point&) const;
	// virtual void move_to(const point&);
	// virtual void put_pixel(const point&) const;
	// virtual void put_char(const point&, const char&) const;
	// virtual void draw_line(const point&, const point&) const;
	// virtual void draw_lineto(const point&) const;
	// virtual void draw_rect(const point&, const size&) const;
	// virtual void draw_text(const point&, const string&) const;

private:

	static constexpr double _fixed_dt = 1.0 / 60.0; // 60 Hz simulation
	static constexpr double _max_frame_time = 0.25; // clamp to avoid spiral of death

	string _title;
	uint32_t _w {0};
	uint32_t _h {0};
	uint32_t _scale {1};
	input_state _input {};

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
	uint64_t _last_counter {0};

	void pump_events();
	void present(const frame_buffer& fb);
	void swap_buffers();
	frame_buffer back_buffer();
};

} // namespace graphics_ns

#endif // __GRAPHICS_H__
