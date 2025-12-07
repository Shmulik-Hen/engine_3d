#include "matrix.h"

// clang-format off
unit unt[mat_dim][mat_dim]={
	{UNIT,  0,     0,     0  },
	{ 0,   UNIT,   0,     0  },
	{ 0,    0,    UNIT,   0  },
	{ 0,    0,     0,    UNIT} };

unit zero[mat_dim][mat_dim]={
	{ 0, 0, 0, 0  },
	{ 0, 0, 0, 0  },
	{ 0, 0, 0, 0  },
	{ 0, 0, 0, 0} };
// clang-format on

matrix UNIT_MAT(unt);

matrix::matrix()
{
	for (int i = 0; i < mat_dim; i++) {
		for (int j = 0; j < mat_dim; j++) {
			mat[i][j] = zero[i][j];
		}
	}
}

matrix::~matrix()
{
}

matrix::matrix(unit n[mat_dim][mat_dim])
{
	for (int i = 0; i < mat_dim; i++) {
		for (int j = 0; j < mat_dim; j++) {
			mat[i][j] = n[i][j];
		}
	}
}

matrix operator*(const matrix &a, const matrix &b)
{
	matrix temp;
	temp.mat[0][0] =
		a.mat[0][0] * b.mat[0][0] + a.mat[0][1] * b.mat[1][0] + a.mat[0][2] * b.mat[2][0] + a.mat[0][3] * b.mat[3][0];
	temp.mat[0][1] =
		a.mat[0][0] * b.mat[0][1] + a.mat[0][1] * b.mat[1][1] + a.mat[0][2] * b.mat[2][1] + a.mat[0][3] * b.mat[3][1];
	temp.mat[0][2] =
		a.mat[0][0] * b.mat[0][2] + a.mat[0][1] * b.mat[1][2] + a.mat[0][2] * b.mat[2][2] + a.mat[0][3] * b.mat[3][2];
	temp.mat[0][3] =
		a.mat[0][0] * b.mat[0][3] + a.mat[0][1] * b.mat[1][3] + a.mat[0][2] * b.mat[2][3] + a.mat[0][3] * b.mat[3][3];
	temp.mat[1][0] =
		a.mat[1][0] * b.mat[0][0] + a.mat[1][1] * b.mat[1][0] + a.mat[1][2] * b.mat[2][0] + a.mat[1][3] * b.mat[3][0];
	temp.mat[1][1] =
		a.mat[1][0] * b.mat[0][1] + a.mat[1][1] * b.mat[1][1] + a.mat[1][2] * b.mat[2][1] + a.mat[1][3] * b.mat[3][1];
	temp.mat[1][2] =
		a.mat[1][0] * b.mat[0][2] + a.mat[1][1] * b.mat[1][2] + a.mat[1][2] * b.mat[2][2] + a.mat[1][3] * b.mat[3][2];
	temp.mat[1][3] =
		a.mat[1][0] * b.mat[0][3] + a.mat[1][1] * b.mat[1][3] + a.mat[1][2] * b.mat[2][3] + a.mat[1][3] * b.mat[3][3];
	temp.mat[2][0] =
		a.mat[2][0] * b.mat[0][0] + a.mat[2][1] * b.mat[1][0] + a.mat[2][2] * b.mat[2][0] + a.mat[2][3] * b.mat[3][0];
	temp.mat[2][1] =
		a.mat[2][0] * b.mat[0][1] + a.mat[2][1] * b.mat[1][1] + a.mat[2][2] * b.mat[2][1] + a.mat[2][3] * b.mat[3][1];
	temp.mat[2][2] =
		a.mat[2][0] * b.mat[0][2] + a.mat[2][1] * b.mat[1][2] + a.mat[2][2] * b.mat[2][2] + a.mat[2][3] * b.mat[3][2];
	temp.mat[2][3] =
		a.mat[2][0] * b.mat[0][3] + a.mat[2][1] * b.mat[1][3] + a.mat[2][2] * b.mat[2][3] + a.mat[2][3] * b.mat[3][3];
	temp.mat[3][0] =
		a.mat[3][0] * b.mat[0][0] + a.mat[3][1] * b.mat[1][0] + a.mat[3][2] * b.mat[2][0] + a.mat[3][3] * b.mat[3][0];
	temp.mat[3][1] =
		a.mat[3][0] * b.mat[0][1] + a.mat[3][1] * b.mat[1][1] + a.mat[3][2] * b.mat[2][1] + a.mat[3][3] * b.mat[3][1];
	temp.mat[3][2] =
		a.mat[3][0] * b.mat[0][2] + a.mat[3][1] * b.mat[1][2] + a.mat[3][2] * b.mat[2][2] + a.mat[3][3] * b.mat[3][2];
	temp.mat[3][3] =
		a.mat[3][0] * b.mat[0][3] + a.mat[3][1] * b.mat[1][3] + a.mat[3][2] * b.mat[2][3] + a.mat[3][3] * b.mat[3][3];
	return temp;
}

matrix &matrix::operator*=(const matrix &m)
{
	*this = *this * m;
	return *this;
}

void prep_gen_mat(matrix &m, const attrib &a)
{
	m.mat[0][0] = a.zoom * cos(a.deg_y) * cos(a.deg_z);
	m.mat[0][1] = a.zoom * -cos(a.deg_y) * sin(a.deg_z);
	m.mat[0][2] = a.zoom * -sin(a.deg_y);
	m.mat[0][3] = a.off_x;
	m.mat[1][0] = a.zoom * (cos(a.deg_x) * sin(a.deg_z) - sin(a.deg_x) * sin(a.deg_y) * cos(a.deg_z));
	m.mat[1][1] = a.zoom * (cos(a.deg_x) * cos(a.deg_z) + sin(a.deg_x) * sin(a.deg_y) * sin(a.deg_z));
	m.mat[1][2] = a.zoom * -sin(a.deg_x) * cos(a.deg_y);
	m.mat[1][3] = a.off_y;
	m.mat[2][0] = a.zoom * (sin(a.deg_x) * sin(a.deg_z) + cos(a.deg_x) * sin(a.deg_y) * cos(a.deg_z));
	m.mat[2][1] = a.zoom * (sin(a.deg_x) * cos(a.deg_z) - cos(a.deg_x) * sin(a.deg_y) * sin(a.deg_z));
	m.mat[2][2] = a.zoom * cos(a.deg_x) * cos(a.deg_y);
	m.mat[2][3] = a.off_z;
	m.mat[3][0] = 0;
	m.mat[3][1] = 0;
	m.mat[3][2] = 0;
	m.mat[3][3] = UNIT;
}

void prep_rot_mat(matrix &m, const attrib &a)
{
	m.mat[0][0] = cos(a.deg_y) * cos(a.deg_z);
	m.mat[0][1] = -cos(a.deg_y) * sin(a.deg_z);
	m.mat[0][2] = -sin(a.deg_y);
	m.mat[0][3] = 0;
	m.mat[1][0] = cos(a.deg_x) * sin(a.deg_z) - sin(a.deg_x) * sin(a.deg_y) * cos(a.deg_z);
	m.mat[1][1] = cos(a.deg_x) * cos(a.deg_z) + sin(a.deg_x) * sin(a.deg_y) * sin(a.deg_z);
	m.mat[1][2] = -sin(a.deg_x) * cos(a.deg_y);
	m.mat[1][3] = 0;
	m.mat[2][0] = sin(a.deg_x) * sin(a.deg_z) + cos(a.deg_x) * sin(a.deg_y) * cos(a.deg_z);
	m.mat[2][1] = sin(a.deg_x) * cos(a.deg_z) - cos(a.deg_x) * sin(a.deg_y) * sin(a.deg_z);
	m.mat[2][2] = cos(a.deg_x) * cos(a.deg_y);
	m.mat[2][3] = 0;
	m.mat[3][0] = 0;
	m.mat[3][1] = 0;
	m.mat[3][2] = 0;
	m.mat[3][3] = UNIT;
}

vector operator*(const matrix &m, const vector &v)
{
	vector temp;
	temp.coord[X] = m.mat[0][0] * v.coord[X] + m.mat[0][1] * v.coord[Y] + m.mat[0][2] * v.coord[Z] + m.mat[0][3];
	temp.coord[Y] = m.mat[1][0] * v.coord[X] + m.mat[1][1] * v.coord[Y] + m.mat[1][2] * v.coord[Z] + m.mat[1][3];
	temp.coord[Z] = m.mat[2][0] * v.coord[X] + m.mat[2][1] * v.coord[Y] + m.mat[2][2] * v.coord[Z] + m.mat[2][3];
	return temp;
}
