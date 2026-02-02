#pragma once

#include <iomanip>
#include <iostream>

const std::string ERR_PFX = "Error: ";
const std::string WARN_PFX = "Warning: ";
const std::string INFO_PFX = "Info: ";
const std::string SEP = ", ";
const std::string PFX = ": ";
const std::string SFX = ";";

#define ENDL      std::endl
#define FLUSH     std::flush
#define FUNC      STR(__PRETTY_FUNCTION__, 1) << PFX
#define HEX(n, w) "0x" << std::hex << std::setw((w)) << std::setfill('0') << std::right << (n)
#define DEC(n, w) std::dec << std::setw((w)) << std::setfill(' ') << std::right << (n)
#define FLT(n, p) std::setprecision((p)) << std::fixed << (n)
#define STR(s, w) std::setw((w)) << std::setfill(' ') << std::left << (s)
#define ERR(s)    std::cerr << STR(ERR_PFX, 1) << s << PFX << FUNC << ENDL << FLUSH
#define WARN(s)   std::cerr << STR(WARN_PFX, 1) << s << PFX << FUNC << ENDL << FLUSH
#define INFO(s)   std::cout << STR(INFO_PFX, 1) << s << ENDL << FLUSH

#ifdef DEBUG_PRINTS
const std::string DBG_PFX = "Debug: ";
#define DBG(s) std::cout << STR(DBG_PFX, 1) << s << ENDL << FLUSH
#else
#define DBG(...)
#endif // DEBUG_PRINTS

// coordinates
constexpr int X_ = 0;
constexpr int Y_ = 1;
constexpr int Z_ = 2;
constexpr int O_ = 3;

constexpr int MIN_VECTORS = 3;
constexpr int NUM_COORDS = 3;
constexpr int NUM_ATTRIBUTES = 7;

