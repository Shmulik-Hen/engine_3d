#pragma once

#include <limits>

typedef float unit;
constexpr unit UNIT(1.0f);
constexpr unit ZERO(0.0f);
constexpr unit EPSILON = 1e-3f;
constexpr unit ZLIMIT(1000.0f);
constexpr unit PI = 3.14159265358979323846f;
constexpr unit DEG_TO_RAD = PI / 180.0f;

// Scale factor for EPSILON when computing the squared tolerance
// for detecting nearly-duplicate polygon vertices.
static constexpr unit CONSEC_TOLERANCE_SCALE = 10.0f;
