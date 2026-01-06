#include "common.h"
#include "element.h"
#include "utils.h"

namespace element_ns
{

using std::cout;
using std::endl;
using std::ios;
using vector_3_ns::vector_3;

element::element()
{
	_polygons.clear();
}

element::~element()
{
	if (_name) {
		delete _name;
	}

	if (_parrent_name) {
		delete _parrent_name;
	}
}

bool element::read(const poly_list& list, element** root, ifstream& f)
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
					polygon* p = find(list, *s);
					if (p) {
						_polygons.push_back(p);
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
				_active = (bool)atoi(line);
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
#ifdef DEBUG_GRFX
	DBG(STR("  element:", 1));
	if (_name) {
		DBG(STR("    name: ", 1) << *_name);
	}

	if (_parrent_name) {
		DBG(STR("    parrent_name: ", 1) << *_parrent_name);
	}

	DBG(STR("    active: ", 1) << _active);
	DBG(STR("     dirty: ", 1) << _dirty);
	_att.print();

	if (!_polygons.empty()) {
		DBG(STR("    polygons:", 1));
		for (const auto poly : _polygons) {
			poly->print();
		}
	}
#endif // DEBUG_GRFX
}

void prn(void* p [[maybe_unused]])
{
#ifdef DEBUG_GRFX
	if (p) {
		element* e = (element*)p;
		e->print();
	}
#endif // DEBUG_GRFX
}

void element::print_all()
{
#ifdef DEBUG_GRFX
	this->print_tree(prn);
#endif // DEBUG_GRFX
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
	_dirty = true;
}

void element::update(const matrix& p_gen, const matrix& p_rot)
{
	vector_3 dist, fill, normal;
	unit view_angle, light_angle;

	if (!_active) {
		return;
	}

	_gen_mat.prep_gen_mat(_att);
	_rot_mat.prep_rot_mat(_att);

	if (!_parrent && !_mats_prepared) {
		// only once for root
		_mats_prepared = true;
	}

	if (_dirty) {
		_gen_mat *= p_gen;
		_rot_mat *= p_rot;

		if (!_polygons.empty()) {
			for (auto poly : _polygons) {
				poly->update(_gen_mat, _rot_mat);
			}
		}

		_dirty = false;
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

polygon* element::find(const poly_list& list, const string& s) const
{
	const string* st;

	for (const auto poly : list) {
		st = poly->get_name();
		if (st && *st == s) {
			return poly;
		}
	}

	return nullptr;
}

} // namespace element_ns
