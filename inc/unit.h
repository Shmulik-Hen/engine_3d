#ifndef __UNIT_H__
#define __UNIT_H__

#include <fstream>
#include <stdint.h>
#include "common.h"

namespace unit_ns
{

using std::ifstream;
using std::istream;
using std::ostream;

class unit
{
	int32_t _num {0};

public:

	unit() {};
	unit(const int32_t&);
	~unit() {};

	bool read(ifstream&);
	void print() const;

	const unit operator+(const unit&) const;
	const unit operator-(const unit&) const;
	const unit operator*(const unit&) const;
	const unit operator/(const unit&) const;
	const unit operator-() const;
	const unit& operator+=(const unit&);
	const unit& operator-=(const unit&);
	const unit& operator*=(const unit&);
	const unit& operator/=(const unit&);
	operator int();
	operator long();

	friend const int mod(const int32_t&);
	friend const unit abs(const unit&);
	friend const unit sin(const unit&);
	friend const unit cos(const unit&);
	friend const unit sqrt(const unit&);
	friend bool operator>(const unit&, const unit&);
	friend bool operator>=(const unit&, const unit&);
	friend bool operator<(const unit&, const unit&);
	friend bool operator<=(const unit&, const unit&);
	friend ostream& operator<<(ostream&, const unit&);
	friend istream& operator>>(istream&, unit&);
};

const unit UNIT(1024);
const unit ZERO(0);

} // namespace unit_ns
#endif //__UNIT_H__
