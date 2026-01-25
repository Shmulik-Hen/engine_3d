#pragma once

#include "common.h"

namespace attrib_ns
{

// using unit_ns::unit;

class attrib
{
	unit _deg_x {ZERO};
	unit _deg_y {ZERO};
	unit _deg_z {ZERO};
	unit _off_x {ZERO};
	unit _off_y {ZERO};
	unit _off_z {ZERO};
	unit _zoom {UNIT};

public:

	attrib() {}
	~attrib() {}
	attrib(const unit, const unit, const unit, const unit, const unit, const unit, const unit);

	bool read(std::ifstream&);
	void print() const;

	unit get_deg_x() const { return _deg_x; };
	unit get_deg_y() const { return _deg_y; };
	unit get_deg_z() const { return _deg_z; };
	unit get_off_x() const { return _off_x; };
	unit get_off_y() const { return _off_y; };
	unit get_off_z() const { return _off_z; };
	unit get_zoom() const { return _zoom; };

	attrib& operator+=(const attrib&);

	friend std::ostream& operator<<(std::ostream&, const attrib&);
	friend std::istream& operator>>(std::istream&, attrib&);
};

} // namespace attrib_ns
