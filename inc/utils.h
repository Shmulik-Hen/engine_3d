#pragma once

#include "common.h"

struct system_error : std::runtime_error
{
	using std::runtime_error::runtime_error;
};

system_error sys_error(const char* s1, const char* s2 = nullptr);
int read_word(std::ifstream&, char*);
void read_remark(std::ifstream&);
