#pragma once

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
// #include <jsoncpp/json/json.h>

const std::string ERR_PFX = "Error: ";
const std::string WARN_PFX = "Warning: ";
const std::string INFO_PFX = "Info: ";
const std::string SEP = ", ";

#define ENDL      std::endl
#define FUNC      STR(__PRETTY_FUNCTION__, 1) << ": "
#define HEX(n, w) "0x" << std::hex << std::setw((w)) << std::setfill('0') << std::right << (n)
#define DEC(n, w) std::dec << std::setw((w)) << std::setfill(' ') << std::right << (n)
#define DBL(n, p) std::setprecision(p) << std::fixed << (n)
#define STR(s, w) std::setw((w)) << std::setfill(' ') << std::left << (s)
#define ERR(s)    std::cerr << STR(ERR_PFX, 1) << s << ": " << FUNC << ENDL << std::flush
#define WARN(s)   std::cerr << STR(WARN_PFX, 1) << s << ": " << FUNC << ENDL << std::flush
#define INFO(s)   std::cout << STR(INFO_PFX, 1) << s << ENDL << std::flush

#ifdef DEBUG_PRINTS
const std::string DBG_PFX = "Debug: ";
#define DBG(s) std::cout << STR(DBG_PFX, 1) << s << std::endl
#else
#define DBG(...)
#endif // DEBUG_PRINTS

constexpr int MAX_NAME = 10;
constexpr int MAX_LINE = 20;

// coordinates
constexpr int X_ = 0;
constexpr int Y_ = 1;
constexpr int Z_ = 2;
constexpr int O_ = 3;

typedef char LINE[MAX_LINE + 1];
typedef char NAME[MAX_NAME + 1];

typedef float unit;
constexpr unit UNIT(1.0f);
constexpr unit ZERO(0.0f);
constexpr unit EPSILON(1e-3f);
constexpr unit ZLIMIT(400.0f);
constexpr unit PI = 3.14159265358979323846f;
constexpr unit DEG_TO_RAD = PI / 180.0f;
