#pragma once

#include <iomanip>
#include <iostream>
#include <cstdint>

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
constexpr uint32_t X_ = 0;
constexpr uint32_t Y_ = 1;
constexpr uint32_t Z_ = 2;
constexpr uint32_t O_ = 3;

constexpr uint32_t MIN_VECTORS = 3;
constexpr uint32_t MIN_MESHES = 1;
constexpr uint32_t NUM_POINTS = 2;
constexpr uint32_t NUM_ATTRIBUTES = 7;
constexpr uint32_t NUM_COORDS = O_;

const uint32_t JSON_VER_MAJ = 0;
const uint32_t JSON_VER_MIN = 0;
const uint32_t JSON_VER_REV = 3;
const uint32_t JSON_VER_FIELDS = 3;
