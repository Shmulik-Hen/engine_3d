#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include "attrib.h"
#include "list.h"
#include "matrix.h"
#include "polyelem.h"
#include "polygon.h"
#include "tree.h"

class element : public treenode
{
	NAME name{""};
	int dirty_flag{0};
	int active_flag{0};
	attrib att;
	matrix gen_mat;
	matrix rot_mat;
	list<polygon> planes;
	friend class ployelem;

public:

	NAME parrent{""};

	element();
	~element();
	element *find_elem(const char *);
	void update(const attrib &);
	friend int elem_comp(const void *);
	friend void update_tree(element *, matrix, matrix);
	friend void printall(element *);
	void read(ifstream &);
	void print();
};

#endif //__ELEMENT_H__
