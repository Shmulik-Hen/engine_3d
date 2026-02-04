// #define DEBUG_PRINTS
#include "attrib.h"
#include "utils.h"

namespace attrib_ns
{

// clang-format off
attrib::attrib(const unit dx, const unit dy, const unit dz,
	       const unit ox, const unit oy, const unit oz,
	       const unit z) :
	_rot_x {dx}, _rot_y {dy}, _rot_z {dz},
	_trans_x {ox}, _trans_y {oy}, _trans_z {oz},
	_zoom {z}
{
}

attrib::attrib(const unit atts[NUM_ATTRIBUTES]) :
	_rot_x {atts[0]}, _rot_y {atts[1]}, _rot_z {atts[2]},
	_trans_x {atts[3]}, _trans_y {atts[4]}, _trans_z {atts[5]},
	_zoom {atts[6]}
{
}
// clang-format on

bool attrib::read(std::ifstream& ifs)
{
	LINE line;

	// clang-format off
	while ((!read_word(ifs, line)) && (!ifs.eof()));
	if (ifs.eof())  return false;
	_rot_x = strtof(line, NULL);

	while ((!read_word(ifs, line)) && (!ifs.eof()));
	if (ifs.eof())  return false;
	_rot_y = strtof(line, NULL);

	while ((!read_word(ifs, line)) && (!ifs.eof()));
	if (ifs.eof())  return false;
	_rot_z = strtof(line, NULL);

	while ((!read_word(ifs, line)) && (!ifs.eof()));
	if (ifs.eof())  return false;
	_trans_x = strtof(line, NULL);

	while ((!read_word(ifs, line)) && (!ifs.eof()));
	if (ifs.eof())  return false;
	_trans_y = strtof(line, NULL);

	while ((!read_word(ifs, line)) && (!ifs.eof()));
	if (ifs.eof())  return false;
	_trans_z = strtof(line, NULL);

	while ((!read_word(ifs, line)) && (!ifs.eof()));
	if (ifs.eof())  return false;
	_zoom = strtof(line, NULL);
	// clang-format on

	return true;
}

void attrib::print() const
{
	DBG("  attrib:"
	    << ENDL << "            rotate x:    " << FLT(_rot_x, 6) << ENDL
	    << "            rotate y:    " << FLT(_rot_y, 6) << ENDL
	    << "            rotate z:    " << FLT(_rot_z, 6) << ENDL
	    << "            translate x: " << FLT(_trans_x, 6) << ENDL
	    << "            translate y: " << FLT(_trans_y, 6) << ENDL
	    << "            translate z: " << FLT(_trans_z, 6) << ENDL
	    << "            zoom:        " << FLT(_zoom, 6) << ENDL);
}

attrib& attrib::operator=(unit atts[NUM_ATTRIBUTES])
{
	_rot_x = atts[0];
	_rot_y = atts[1];
	_rot_z = atts[2];
	_trans_x = atts[3];
	_trans_y = atts[4];
	_trans_z = atts[5];
	_zoom = atts[6];
	return *this;
}

attrib& attrib::operator+=(const attrib& a)
{
	_rot_x += a._rot_x;
	_rot_y += a._rot_y;
	_rot_z += a._rot_z;
	_trans_x += a._trans_x;
	_trans_y += a._trans_y;
	_trans_z += a._trans_z;
	_zoom *= a._zoom;
	return *this;
}

std::ostream& operator<<(std::ostream& os, const attrib& a)
{
	return os << '(' << a._rot_x << ',' << a._rot_y << ',' << a._rot_z << ','
	          << a._trans_x << ',' << a._trans_y << ',' << a._trans_z << ',' << a._zoom << ')';
}

std::istream& operator>>(std::istream& is, attrib& a)
{
	is >> a._rot_x;
	is >> a._rot_y;
	is >> a._rot_z;
	is >> a._trans_x;
	is >> a._trans_y;
	is >> a._trans_z;
	is >> a._zoom;
	return is;
}

} // namespace attrib_ns
