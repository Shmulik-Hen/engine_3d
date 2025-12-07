#ifndef __POLYELEM_H__
#define __POLYELEM_H__

#include "matrix.h"
#include "polygon.h"
#include "queue.h"
#include "unit.h"
#include "vector.h"

class polyelem : public queue
{
	polyelem *merge(polyelem *, polyelem *);

public:

	matrix mat;
	polygon *poly{NULL};
	char color{0};
	unit depth{0};
	polyelem *next{NULL};

	polyelem();
	~polyelem();
	polyelem *merge_sort();
	void print();
	void show();
};

#endif //__POLYELEM_H__
