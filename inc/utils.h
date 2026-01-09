#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include "common.h"

using std::ifstream;
using std::string;

struct system_error : std::runtime_error
{
	using std::runtime_error::runtime_error;
};

system_error sys_error(const char* s1, const char* s2 = nullptr);
int read_word(ifstream&, char*);
void read_remark(ifstream&);
#endif
