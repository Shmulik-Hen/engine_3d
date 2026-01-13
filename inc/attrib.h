#pragma once

#include "common.h"
#include "unit.h"

namespace attrib_ns
{

using std::ifstream;
using std::istream;
using std::ostream;
using unit_ns::unit;

class attrib
{
	unit _deg_x {unit_ns::ZERO};
	unit _deg_y {unit_ns::ZERO};
	unit _deg_z {unit_ns::ZERO};
	unit _off_x {unit_ns::ZERO};
	unit _off_y {unit_ns::ZERO};
	unit _off_z {unit_ns::ZERO};
	unit _zoom {unit_ns::ZERO};

public:

	attrib() {}
	~attrib() {}
	attrib(const unit, const unit, const unit, const unit, const unit, const unit, const unit);

	bool read(ifstream&);
	void print() const;

	const unit get_deg_x() const { return _deg_x; };
	const unit get_deg_y() const { return _deg_y; };
	const unit get_deg_z() const { return _deg_z; };
	const unit get_off_x() const { return _off_x; };
	const unit get_off_y() const { return _off_y; };
	const unit get_off_z() const { return _off_z; };
	const unit get_zoom() const { return _zoom; };

	attrib& operator+=(const attrib&);

	friend ostream& operator<<(ostream&, const attrib&);
	friend istream& operator>>(istream&, attrib&);
};

} // namespace attrib_ns
