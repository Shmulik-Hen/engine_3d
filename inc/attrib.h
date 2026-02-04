#pragma once

#include "common.h"
#include "unit.h"

namespace attrib_ns
{

class attrib
{
	unit _rot_x {ZERO};   // x-axis rotation
	unit _rot_y {ZERO};   // y-axis rotation
	unit _rot_z {ZERO};   // z-axis rotation
	unit _trans_x {ZERO}; // x-axis translation
	unit _trans_y {ZERO}; // y-axis translation
	unit _trans_z {ZERO}; // z-axis translation
	unit _zoom {UNIT};

public:

	attrib() {}
	~attrib() {}
	attrib(const unit, const unit, const unit, const unit, const unit, const unit, const unit);
	attrib(const unit[NUM_ATTRIBUTES]);

	bool read(std::ifstream&);
	void print() const;

	unit get_rot_x() const { return _rot_x; };
	unit get_rot_y() const { return _rot_y; };
	unit get_rot_z() const { return _rot_z; };
	unit get_trans_x() const { return _trans_x; };
	unit get_trans_y() const { return _trans_y; };
	unit get_trans_z() const { return _trans_z; };
	unit get_zoom() const { return _zoom; };

	attrib& operator=(unit[NUM_ATTRIBUTES]);
	attrib& operator+=(const attrib&);

	friend std::ostream& operator<<(std::ostream&, const attrib&);
	friend std::istream& operator>>(std::istream&, attrib&);
};

} // namespace attrib_ns
