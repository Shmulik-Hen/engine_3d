#include "attrib.h"

namespace attrib_ns
{

attrib::attrib(const unit dx, const unit dy, const unit dz, const unit ox, const unit oy, const unit oz, const unit z) :
	_deg_x {dx}, _deg_y {dy}, _deg_z {dz}, _off_x {ox},
	_off_y {oy}, _off_z {oz}, _zoom {z}
{
}

bool attrib::read(ifstream& f)
{
	bool rc = false;

	rc += _deg_x.read(f);
	rc += _deg_y.read(f);
	rc += _deg_z.read(f);
	rc += _off_x.read(f);
	rc += _off_y.read(f);
	rc += _off_z.read(f);
	rc += _zoom.read(f);
	if (!rc) {
		printf("attrib::read error\n");
		fflush(stdout);
	}
	return rc;
}

void attrib::print() const
{
	printf("    attrib:\n");
	fflush(stdout);
	_deg_x.print();
	_deg_y.print();
	_deg_z.print();
	_off_x.print();
	_off_y.print();
	_off_z.print();
	_zoom.print();
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

ostream& operator<<(ostream& o, const attrib& a)
{
	return o << '(' << a._deg_x << ',' << a._deg_y << ',' << a._deg_z << ','
	         << a._off_x << ',' << a._off_y << ',' << a._off_z << ',' << a._zoom << ')';
}

istream& operator>>(istream& i, attrib& a)
{
	i >> a._deg_x;
	i >> a._deg_y;
	i >> a._deg_z;
	i >> a._off_x;
	i >> a._off_y;
	i >> a._off_z;
	i >> a._zoom;
	return i;
}

} // namespace attrib_ns
