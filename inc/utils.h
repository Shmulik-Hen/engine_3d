#pragma once

#include <fstream>

constexpr int MAX_NAME = 10;
constexpr int MAX_LINE = 20;
typedef char LINE[MAX_LINE + 1];
typedef char NAME[MAX_NAME + 1];

struct system_error : std::runtime_error
{
	using std::runtime_error::runtime_error;
};

system_error sys_error(const char* s1, const char* s2 = nullptr);
int read_word(std::ifstream&, char*);
void read_remark(std::ifstream&);
