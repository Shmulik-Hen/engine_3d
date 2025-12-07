#include "attrib.h"

attrib::attrib()
{
}

attrib::~attrib()
{
}

attrib::attrib(unit dx, unit dy, unit dz, unit ox, unit oy, unit oz, unit z) :
	deg_x{dx}, deg_y{dy}, deg_z{dz}, off_x{ox}, off_y{oy}, off_z{oz}, zoom{z}
{
}

attrib operator+(const attrib &a, const attrib &b)
{
	attrib temp = a;
	temp += b;
	return temp;
}

attrib &attrib::operator+=(const attrib &a)
{
	deg_x += a.deg_x;
	deg_y += a.deg_y;
	deg_z += a.deg_z;
	off_x += a.off_x;
	off_y += a.off_y;
	off_z += a.off_z;
	zoom *= a.zoom;
	return *this;
}

ostream &operator<<(ostream &o, const attrib &a)
{
	return o << '(' << a.deg_x << ',' << a.deg_y << ',' << a.deg_z << ',' << a.off_x << ',' << a.off_y << ',' << a.off_z << ','
		 << a.zoom << ')';
}

istream &operator>>(istream &i, attrib &a)
{
	i >> a.deg_x;
	i >> a.deg_y;
	i >> a.deg_z;
	i >> a.off_x;
	i >> a.off_y;
	i >> a.off_z;
	i >> a.zoom;
	return i;
}

void attrib::read(ifstream &f)
{
	printf("attrib: \n");
	deg_x.read(f);
	deg_y.read(f);
	deg_z.read(f);
	off_x.read(f);
	off_y.read(f);
	off_z.read(f);
	zoom.read(f);
}
void attrib::print()
{
	printf("attrib:\n");
	deg_x.print();
	deg_y.print();
	deg_z.print();
	off_x.print();
	off_y.print();
	off_z.print();
	zoom.print();
}
