#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "attrib.h"
#include "unit.h"
#include "vector.h"

const int mat_dim = 4;
class matrix
{
	unit mat[mat_dim][mat_dim];

public:

	matrix();
	~matrix();
	matrix(unit[mat_dim][mat_dim]);
	matrix &operator*=(const matrix &);
	friend matrix operator*(const matrix &a, const matrix &b);
	friend void prep_gen_mat(matrix &, const attrib &);
	friend void prep_rot_mat(matrix &, const attrib &);
	friend vector operator*(const matrix &, const vector &);
};

#endif //__MATRIX_H__
