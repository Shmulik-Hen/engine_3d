#pragma once

#include <fstream>

struct system_error : std::runtime_error
{
	using std::runtime_error::runtime_error;
};

system_error sys_error(const char* s1, const char* s2 = nullptr);
