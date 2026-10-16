#pragma once

#include <cstdint>

// coordinates
constexpr uint32_t X_ = 0;
constexpr uint32_t Y_ = 1;
constexpr uint32_t Z_ = 2;
constexpr uint32_t O_ = 3;

constexpr uint32_t MIN_VECTORS = 3;
constexpr uint32_t MAX_VECTORS = 8;
constexpr uint32_t MIN_MESHES = 1;
constexpr uint32_t NUM_POINTS = 2;
constexpr uint32_t NUM_ATTRIBUTES = 7;
constexpr uint32_t NUM_COORDS = O_;

const uint32_t JSON_VER_MAJ = 0;
const uint32_t JSON_VER_MIN = 0;
const uint32_t JSON_VER_REV = 3;
const uint32_t JSON_VER_FIELDS = 3;
