#include <iostream>
#include <ios>
#include <stdio.h>

#include "common.h"
#include "element.h"
#include "utils.h"

namespace element_ns
{

using my_vector_ns::my_vector;
using std::cout;
using std::endl;
using std::ios;
// using namespace unit_ns;

my_vector view(0, 0, -1000000);
my_vector n_light(0, 0, -1024);

element::element()
{
	_polygons = new poly_list;
	_polygons->clear();
}

element::~element()
{
	if (_polygons) {
		delete _polygons;
	}

	if (_name) {
		delete _name;
	}

	if (_parrent_name) {
		delete _parrent_name;
	}
}

char make_color(char c1, unit c2)
{
	long color = (long)c2;
	if (color >= 1024) {
		color = 1023;
	}
	return (char)(((color >> 2) & 0x00f0) | (c1 & 0x0F));
}

bool element::read(poly_list* list, element** root, ifstream& f)
{
	LINE line;
	int finish = 0, len;
	bool rc, ret = true;

	while (!f.eof() && !finish) {
		rc = true;
		while ((!read_word(f, line)) && (!f.eof()));

		if (f.eof()) {
			break;
		}

		switch (line[1]) {
		case 'n':
			len = read_word(f, line);
			if (len) {
				_name = new string(line);
				if (!_name) {
					ERR("element::read allocation error -  _name");
					rc = false;
				}
			}
			else {
				ERR("element::read error _name");
				rc = false;
			}
			break;
		case 't':
			if (root) {
				if (!*root) {
					*root = this;
				}
				else {
					len = read_word(f, line);
					if (len) {
						_parrent_name = new string(line);
						if (_parrent_name) {
							element* parrent = find(*root, *_parrent_name);
							if (parrent) {
								add_node(parrent);
								_parrent = parrent;
							}
							else {
								ERR("element::read error -  _parrent_name");
								rc = false;
							}
						}
						else {
							ERR("element::read allocation error -  _parrent_name");
							rc = false;
						}
					}
				}
			}
			else {
				ERR("element::read root is NULL");
				rc = false;
			}
			break;
		case 'p':
			len = read_word(f, line);
			if (len) {
				string* s = new string(line);
				if (s) {
					const polygon* p = find(list, *s);
					if (p) {
						_polygons->push_front(*p);
					}
					else {
						ERR("element::read find error -  polygon");
						rc = false;
					}
				}
				else {
					ERR("element::read allocation error -  polygon");
					rc = false;
				}
			}
			else {
				ERR("element::read error polygon");
				rc = false;
			}
			break;
		case 'f':
			len = read_word(f, line);
			if (len) {
				_active = atoi(line);
				_dirty = 0;
			}
			else {
				ERR("element::read error flag");
				rc = false;
			}
			break;
		case 'a':
			rc = _att.read(f);
			if (!rc) {
				ERR("element::read error attrib");
				rc = false;
			}
			break;
		default:
			finish = 1;
			f.seekg(-4, ios::cur);
			break;
		}

		if (!rc) {
			ERR("element::read parsing error");
			ret = false;
		}
	}

	return ret;
}

void element::print() const
{
	DBG(STR("  element:", 1));
	if (_name) {
		DBG(STR("    name: ", 1) << *_name);
	}

	if (_parrent_name) {
		DBG(STR("    parrent_name: ", 1) << *_parrent_name);
	}

	DBG(STR("    active: ", 1) << DEC(_active, 4));
	DBG(STR("     dirty: ", 1) << DEC(_dirty, 4));
	_att.print();

	if (!_polygons->empty()) {
		DBG(STR("    polygons:", 1));
		for (pol_it it = _polygons->begin(); it != _polygons->end(); ++it) {
			const polygon* p = &*it;
			if (p) {
				p->print();
			}
		}
	}
}

void prn(void* p)
{
	if (p) {
		element* e = (element*)p;
		e->print();
	}
}

void element::print_all()
{
	this->print_tree(prn);
}

string cmp_str;

bool cmp(void* p)
{
	element* e = (element*)p;

	if (e && e->get_name()) {
		return (*e->get_name() == cmp_str);
	}

	return false;
}

element* element::find(element* root, string& s) const
{
	cmp_str = s;
	return root->search_tree(cmp);
}

void element::update(const attrib& att)
{
	_att += att;
	_dirty = 1;
}

void element::update(const matrix& p_gen, const matrix& p_rot)
{
	my_vector dist, fill, normal;
	unit view_angle, light_angle;
	char color;

	_gen_mat.prep_gen_mat(_att);
	_rot_mat.prep_rot_mat(_att);
	_gen_mat *= p_gen;
	_rot_mat *= p_rot;

	for (pol_it it = _polygons->begin(); it != _polygons->end(); ++it) {
		polygon* p = &*it;
		if (!p) {
			return;
		}

		fill = _gen_mat * p->get_fill();
		p->set_fill(fill);
		normal = _rot_mat * p->get_normal();
		p->set_normal(normal);

		dist = fill - view;
		view_angle = normal * dist;
		if ((view_angle < unit_ns::ZERO) || p->get_force()) {
			light_angle = normal * n_light;
			if ((light_angle > unit_ns::ZERO) || p->get_force()) {
				p->set_depth(dist * dist);
				color = make_color(p->get_color(), abs(light_angle));
				p->set_color(color);
			}
		}
	}
}

void element::update()
{
	matrix m_gen, m_rot;

	if (_parrent) {
		m_gen = _parrent->_gen_mat;
		m_rot = _parrent->_rot_mat;
	}
	else {
		m_gen = matrix_ns::get_unit_mat();
		m_rot = matrix_ns::get_unit_mat();
	}

	update(m_gen, m_rot);
}

void upd(void* p)
{
	if (p) {
		element* e = (element*)p;
		e->update();
	}
}

void element::update_all()
{
	this->update_tree(upd);
}

const polygon* element::find(poly_list* list, const string& s) const
{
	if (!list) {
		return nullptr;
	}

	for (pol_it it = list->begin(); it != list->end(); ++it) {
		const polygon* p = &*it;

		if (p && p->get_name() && *p->get_name() == s) {
			return p;
		}
	}

	return nullptr;
}

} // namespace element_ns
