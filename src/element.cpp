#include <string.h>
#include <iostream>
#include <ios>
#include <stdio.h>
#include "element.h"
#include "polygon.h"
#include "utils.h"

using std::cout;
using std::endl;
using std::ios;

NAME elem_comp_name;
// const list<polygon> poly_list;
extern vector view;
extern vector n_light;
// extern matrix UNIT_MAT;

char make_color(char c1, unit c2)
{
	long color = (long)c2;
	if (color >= 1024)
		color = 1023;
	return (char)(((color >> 2) & 0x00f0) | (c1 & 0x0F));
}

element::element()
{
	elem_comp_name[0] = '\0';
}

element::~element()
{
}

int elem_comp(const void *node)
{
	element *p = (element *)node;
	return (!strncmp(elem_comp_name, p->name, MAX_NAME));
}

element *element::find_elem(const char *s)
{
	snprintf(elem_comp_name, MAX_NAME, "%s", s);
	return (element *)this->search(this, elem_comp);
}

void element::update(const attrib &a)
{
	att += a;
}

void update_tree(element *node, matrix p_gen, matrix p_rot)
{
	polyelem *pe;
	polygon *poly;
	vector dist, fill, normal;
	unit view_angle, light_angle;

	if (node) {
		if (node->active_flag) {
			prep_gen_mat(node->gen_mat, node->att);
			prep_rot_mat(node->rot_mat, node->att);
			node->gen_mat = p_gen * node->gen_mat;
			node->rot_mat = p_rot * node->rot_mat;
			for (poly = node->planes.first(); poly; poly = node->planes.next()) {
				fill = node->gen_mat * poly->fill;
				normal = node->rot_mat * poly->normal;
				dist = fill - view;
				view_angle = normal * dist;
				if ((view_angle < ZERO) || poly->force) {
					light_angle = normal * n_light;
					if ((light_angle > ZERO) || poly->force) {
						pe = new polyelem;
						pe->depth = dist * dist;
						pe->poly = poly;
						pe->mat = node->gen_mat;
						pe->color = make_color(poly->color, abs(light_angle));
						pe->push();
					}
				}
			}
			if (node->son)
				update_tree((element *)node->son, node->gen_mat, node->rot_mat);
		}
		if (node->next)
			update_tree((element *)node->next, p_gen, p_rot);
	}
}

void element::read(ifstream &f)
{
	LINE line;
	polygon *po;
	int finish = 0;

	while (!f.eof() && !finish) {
		printf("element: \n");
		while ((!read_word(f, line)) && (!f.eof()))
			;
		if (f.eof())
			break;
		switch (line[1]) {
		case 'n':
			read_word(f, line);
			strcpy(name, line);
			printf("element: n: %s, %s\n", line, name);
			break;
		case 'f':
			read_word(f, line);
			strcpy(parrent, line);
			printf("element: f: %s, %s\n", line, parrent);
			break;
		case 't':
			read_word(f, line);
			active_flag = atoi(line);
			dirty_flag = 0;
			printf("element: t: %s, %d\n", line, active_flag);
			break;
		case 'a':
			printf("element: a: \n");
			att.read(f);
			break;
		case 's':
			read_word(f, line);
			printf("element: s: %s\n", line);
			po = find_poly(polygon::poly_list, line);
			if (po)
				planes.insert(po);
			else
				error("polygon not found in element::read()", line);
			break;
		default:
			printf("element: def: \n");
			finish = 1;
			f.seekg(-4, ios::cur);
			break;
		}
	}
}

void element::print()
{
	polygon *p;

	cout << name << endl;
	cout << parrent << endl;
	cout << att << endl;
	for (p = planes.first(); p; p = planes.next())
		p->print();
}

void printall(element *root)
{
	if (root) {
		root->print();
		printall((element *)root->son);
		printall((element *)root->next);
	}
}
