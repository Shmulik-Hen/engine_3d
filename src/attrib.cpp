// #define DEBUG_PRINTS
#include "attrib.h"
#include "utils.h"

namespace attrib_ns
{

// clang-format off
attrib::attrib(const unit dx, const unit dy, const unit dz,
	       const unit ox, const unit oy, const unit oz,
	       const unit z) :
	_deg_x {dx}, _deg_y {dy}, _deg_z {dz},
	_off_x {ox}, _off_y {oy}, _off_z {oz},
	_zoom {z}
{
}
// clang-format on

bool attrib::read(std::ifstream& ifs)
{
	LINE line;

	while (!read_word(ifs, line));
	_deg_x = strtof(line, NULL);
	while (!read_word(ifs, line));
	_deg_y = strtof(line, NULL);
	while (!read_word(ifs, line));
	_deg_z = strtof(line, NULL);
	while (!read_word(ifs, line));
	_off_x = strtof(line, NULL);
	while (!read_word(ifs, line));
	_off_y = strtof(line, NULL);
	while (!read_word(ifs, line));
	_off_z = strtof(line, NULL);
	while (!read_word(ifs, line));
	_zoom = strtof(line, NULL);
	return true;
}

void attrib::print() const
{
#ifdef DEBUG_PRINTS
	DBG("    attrib:" << ENDL
	<< "deg x: "_<< _deg_x << ENDL
	<< "deg y: "_<< _deg_y << ENDL
	<< "deg z: "_<< _deg_z << ENDL
	<< "off x: "_<< _off_x << ENDL
	<< "off y: "_<< _off_y << ENDL
	<< "off z: "_<< _off_z << ENDL
	<< "zoom: "_<< _zoom << ENDL
#endif // DEBUG_PRINTS
}

attrib& attrib::operator+=(const attrib& a)
{
	_deg_x += a._deg_x;
	_deg_y += a._deg_y;
	_deg_z += a._deg_z;
	_off_x += a._off_x;
	_off_y += a._off_y;
	_off_z += a._off_z;
	_zoom *= a._zoom;
	return *this;
}

std::ostream& operator<<(std::ostream& os, const attrib& a)
{
	return os << '(' << a._deg_x << ',' << a._deg_y << ',' << a._deg_z << ','
	          << a._off_x << ',' << a._off_y << ',' << a._off_z << ',' << a._zoom << ')';
}

std::istream& operator>>(std::istream& is, attrib& a)
{
	is >> a._deg_x;
	is >> a._deg_y;
	is >> a._deg_z;
	is >> a._off_x;
	is >> a._off_y;
	is >> a._off_z;
	is >> a._zoom;
	return is;
}

} // namespace attrib_ns
