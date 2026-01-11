// #define DEBUG_PRINTS
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

bool element::read(const polygon::poly_list& list, element** root, ifstream& f)
{
	LINE line;
	int finish = 0, len;

	while (!f.eof() && !finish) {
		bool rc;

		while ((!read_word(f, line)) && (!f.eof()));

		if (f.eof()) {
			break;
		}

		switch (line[1]) {
		case 'n':
			// read element's name
			len = read_word(f, line);
			if (!len) {
				sys_error("element::read error _name");
			}

			_name = new string(line);
			DBG("read: " << *_name);
			break;
		case 't':
			if (!root) {
				sys_error("element::read root is NULL");
			}

			if (!*root) {
				// root has no parrent
				*root = this;
			}
			else {
				// all others must have a parrent
				len = read_word(f, line);
				if (!len) {
					sys_error("element::read read error -  _parrent_name");
				}

				_parrent_name = new string(line);
				element* parrent = find(*root, *_parrent_name);
				if (!parrent) {
					delete _parrent_name;
					sys_error("element::read find error -  _parrent_name");
				}

				add_node(parrent);
				_parrent = parrent;
			}
			break;
		case 'p':
			// add a polygon to element
			len = read_word(f, line);
			if (len) {
				string* s = new string(line);
				polygon* p = find(list, *s);
				if (!p) {
					delete s;
					sys_error("element::read find error -  polygon");
				}

				_polygons.push_back(p);
				DBG("read: " << *p->get_name());
			}
			else {
				sys_error("element::read error polygon");
			}
			break;
		case 'f':
			// force flag
			len = read_word(f, line);
			if (!len) {
				sys_error("element::read error flag");
			}

			_active = (bool)atoi(line);
			break;
		case 'a':
			// element's attribute
			rc = _att.read(f);
			if (!rc) {
				sys_error("element::read error attrib");
			}
			break;
		default:
			finish = 1;
			f.seekg(-4, ios::cur);
			break;
		}
	}

	return true;
}

void element::print() const
{
#ifdef DEBUG_PRINTS
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
#endif // DEBUG_PRINTS
}

void prn(void* p [[maybe_unused]])
{
#ifdef DEBUG_PRINTS
	if (p) {
		element* e = (element*)p;
		e->print();
	}
#endif // DEBUG_PRINTS
}

void element::print_all()
{
#ifdef DEBUG_PRINTS
	this->print_tree(prn);
#endif // DEBUG_PRINTS
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

		if (_polygons.size()) {
			for (auto poly : _polygons) {
				poly->update(_gen_mat, _rot_mat);
			}
		}
		else {
			DBG("_polygons is empty");
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

polygon* element::find(const polygon::poly_list& list, const string& s) const
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
