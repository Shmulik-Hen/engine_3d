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
	int32_t num {0};

public:

	unit();
	unit(const int32_t&);
	~unit() {};
	unit operator+(const unit&) const;
	unit operator-(const unit&) const;
	unit operator*(const unit&) const;
	unit operator/(const unit&) const;
	unit operator-() const;
	unit& operator+=(const unit&);
	unit& operator-=(const unit&);
	unit& operator*=(const unit&);
	unit& operator/=(const unit&);
	operator int();
	operator long();
	int32_t convert(const char*);
	bool read(ifstream&);
	void print() const;

	friend int operator>(const unit&, const unit&);
	friend int operator>=(const unit&, const unit&);
	friend int operator<(const unit&, const unit&);
	friend int mod(const int32_t&);
	friend unit abs(const unit&);
	friend unit sin(const unit&);
	friend unit cos(const unit&);
	friend unit sqrt(const unit&);
	friend ostream& operator<<(ostream&, const unit&);
	friend istream& operator>>(istream&, unit&);
};

const unit UNIT(1024);
const unit ZERO(0);

} // namespace unit_ns
#endif //__UNIT_H__
