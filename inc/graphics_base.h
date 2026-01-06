#ifndef __GRAPHICS_BASE_H__
#define __GRAPHICS_BASE_H__

#include "common.h"

namespace graphics_base_ns
{

class graphics_base
{
public:

	// enum color_idx
	// {
	// 	__first_color__ = 0, // Dummy, not an qctual color

	// 	black = __first_color__,
	// 	white,
	// 	grey,
	// 	dark_grey,

	// 	bright_red,
	// 	red,
	// 	dark_red,

	// 	bright_orange,
	// 	orange,
	// 	dark_orange,

	// 	bright_yellow,
	// 	yellow,
	// 	dark_yellow,

	// 	bright_green,
	// 	green,
	// 	dark_green,

	// 	bright_blue,
	// 	blue,
	// 	dark_blue,

	// 	bright_purple,
	// 	purple,
	// 	dark_purple,

	// 	__last_color__ // Dummy, not an qctual color
	// };

	// enum bounds_status
	// {
	// 	BOUNDS_OK = 0x00000000,
	// 	BOUNDS_X_OUT = 0x00000001,
	// 	BOUNDS_Y_OUT = 0x00000002,
	// 	BOUNDS_BOTH_OUT = (BOUNDS_X_OUT | BOUNDS_Y_OUT)
	// };

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

	virtual ~graphics_base() {};
	virtual void init_graphics() = 0;
	virtual void close_graphics() = 0;
	// virtual void set_viewport(const point&, const size&) = 0;
	// virtual void clear_viewport() = 0;
	// virtual void refresh() const = 0;
	// virtual bool is_in_bounds(point) const = 0;
	// virtual uint32_t get_width() const = 0;
	// virtual uint32_t get_height() const = 0;
};

} // namespace graphics_base_ns

#endif // __GRAPHICS_BASE_H__
