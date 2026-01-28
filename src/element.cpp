// #define DEBUG_PRINTS
#include "common.h"
#include "element.h"
#include "utils.h"

namespace element_ns
{

using namespace matrix_ns;
using namespace polygon_ns;
using polylist_t = polygon_ns::polygon::polylist_t;

bool element_ns::element::_mats_prepared = false;

bool element::read(const polylist_t& poly_list, element* root, std::ifstream& ifs)
{
	LINE line;
	int len;
	element* parrent;
	polygon* poly;
	std::string s;
	bool rc, finish = false;

	while (!ifs.eof() && !finish) {
		while ((!read_word(ifs, line)) && (!ifs.eof()));
		if (ifs.eof()) {
			break;
		}

		switch (line[1]) {
		case 'n':
			// read element's name
			len = read_word(ifs, line);
			if (!len) {
				sys_error("element::read error _name");
			}

			_name = line;
			break;
		case 't':
			// read element's parrent name
			len = read_word(ifs, line);
			if (len) {
				_parrent_name = line;
				parrent = find(root, _parrent_name);
				if (!parrent) {
					sys_error("element::read find error - _parrent_name");
				}

				add_node(parrent);
				_parrent = parrent;
			}
			break;
		case 'p':
			// add a polygon to element
			len = read_word(ifs, line);
			if (len) {
				s = line;
				poly = find(poly_list, s);
				if (!poly) {
					sys_error("element::read find error -  polygon");
				}
				_polygons.push_back(poly);
			}
			else {
				sys_error("element::read error polygon");
			}
			break;
		case 'f':
			// element active flag
			len = read_word(ifs, line);
			if (!len) {
				sys_error("element::read error flag");
			}

			_active = (bool)atoi(line);
			break;
		case 'a':
			// element's attribute
			rc = _att.read(ifs);
			if (!rc) {
				sys_error("element::read error attrib");
			}
			break;
		default:
			finish = true;
			ifs.seekg(-4, std::ios::cur);
			break;
		}
	}

	return true;
}

void element::print() const
{
#ifdef DEBUG_PRINTS
	DBG(STR("  element:", 1));
	DBG(STR("    name: ", 1) << _name);
	DBG(STR("    parrent_name: ", 1) << _parrent_name);
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
	element* e = (element*)p;
	if (e) {
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

std::string cmp_str;

bool cmp(void* p)
{
	element* e = (element*)p;

	if (e) {
		return (e->get_name() == cmp_str);
	}

	return false;
}

element* element::find(element* root, const std::string& s) const
{
	cmp_str = s;
	return root->search_tree(cmp);
}

void element::update(const attrib& att)
{
	_att += att;
	_dirty = true;
}

void element::update(const matrix& p_trans, const matrix& p_rot)
{
	if (!_active) {
		return;
	}

	if (!_parrent) {
		// only once for root
		if (!_mats_prepared) {
			_trans_mat.prep_trans_mat(_att);
			_rot_mat.prep_rot_mat(_att);
			_mats_prepared = true;
		}
	}
	else {
		_trans_mat.prep_trans_mat(_att);
		_rot_mat.prep_rot_mat(_att);
	}

	if (_dirty) {
		_trans_mat *= p_trans;
		_rot_mat *= p_rot;

		if (_polygons.size()) {
			for (auto poly : _polygons) {
				if (poly) {
					poly->update(_trans_mat, _rot_mat);
				}
			}
		}

		_dirty = false;
	}
}

void element::update()
{
	matrix m_trans, m_rot;

	if (_parrent) {
		m_trans = _parrent->_trans_mat;
		m_rot = _parrent->_rot_mat;
	}
	else {
		m_trans = matrix_ns::get_unit_mat();
		m_rot = matrix_ns::get_unit_mat();
	}

	update(m_trans, m_rot);
}

void upd(void* p)
{
	element* e = (element*)p;
	if (e) {
		e->update();
	}
}

void element::update_all()
{
	this->update_tree(upd);
}

polygon* element::find(const polylist_t& poly_list, const std::string& s) const
{
	for (const auto poly : poly_list) {
		if (poly && poly->get_name() == s) {
			return poly;
		}
	}

	return nullptr;
}

} // namespace element_ns
