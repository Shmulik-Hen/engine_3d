#include <iostream>
#include <ios>
#include <string.h>
#include <stdlib.h>
#include "polygon.h"
#include "utils.h"

using std::cout;
using std::endl;
using std::ios;

list<polygon> polygon::poly_list;
NAME poly_comp_name;

polygon::polygon()
{
	poly_comp_name[0] = '\0';
}

polygon::~polygon()
{
}

vector polygon::find_fill()
{
	vector *p, temp;
	long num = 0;

	temp -= temp;
	for (p = points.first(); p; p = points.next()) {
		temp += *p;
		num++;
	}
	unit n(num << 10);
	temp.coord[X] /= n;
	temp.coord[Y] /= n;
	temp.coord[Z] /= n;
	return temp;
}

vector polygon::find_normal()
{
	vector *v1, *v2, v;
	v1 = points.first();
	v2 = points.next();
	v.coord[X] = (v1->coord[Y] * v2->coord[Z] - v1->coord[Z] * v2->coord[Y]);
	v.coord[Y] = (v1->coord[Z] * v2->coord[X] - v1->coord[X] * v2->coord[Z]);
	v.coord[Z] = (v1->coord[X] * v2->coord[Y] - v1->coord[Y] * v2->coord[X]);
	normalize(v);
	return v;
}

int poly_comp(const void *link)
{
	polygon *p = (polygon *)link;
	return (!strncmp(poly_comp_name, p->name, MAX_NAME));
}

polygon *find_poly(list<polygon> &lst, char *s)
{
	snprintf(poly_comp_name, MAX_NAME, "%s", s);
	return lst.search(poly_comp);
}
/*
int name_comp(const polygon& p,NAME n)
{
 return strcmp(p.name,n);
} */

void polygon::read(ifstream &f)
{
	LINE line;
	vector *vp;
	int finish = 0;

	while (!f.eof() && !finish) {
		printf("polygon: \n");
		while ((!read_word(f, line)) && (!f.eof()))
			;
		if (f.eof())
			break;
		switch (line[1]) {
		case 'n':
			read_word(f, line);
			strcpy(name, line);
			printf("polygon: n: %s, %s\n", line, name);
			break;
		case 'c':
			read_word(f, line);
			color = atoi(line);
			printf("polygon: c: %s, %d\n", line, color);
			break;
		case 'f':
			read_word(f, line);
			force = atoi(line);
			printf("polygon: f: %s, %d\n", line, force);
			break;
		case 'o':
			printf("polygon: o: \n");
			normal.read(f);
			break;
		case 'v':
			printf("polygon: v: \n");
			vp = new vector;
			vp->read(f);
			points.insert(vp);
			break;
		default:
			printf("polygon: def: \n");
			finish = 1;
			f.seekg(-4, ios::cur);
			break;
		}
	}
	fill = find_fill();
	// normal=find_normal();
}

void polygon::print()
{
	vector *p;
#if 0
	cout << name << endl;
	// cout<<color<<endl;
	// cout<<force<<endl;
	cout << "fill:" << fill << endl;
	cout << "normal:" << normal << endl;
	for (p = points.first(); p; p = points.next())
		cout << *p << endl;
#else
	printf("      polygon:\n");
	printf("      name: %s\n", name);
	printf("      force: %d\n", (int)force);
	printf("      color: %c\n", color);
	printf("      fill:\n");
	fill.print();
	printf("      normal:\n");
	normal.print();
	printf("      points:\n");
	for (p = points.first(); p; p = points.next())
		p->print();
#endif
}
