#pragma once

#include <limits>

typedef float unit;
constexpr unit UNIT(1.0f);         // a '1' in engines units
constexpr unit ZERO(0.0f);         // a '0' in engines units
constexpr unit EPSILON(1e-5f);     // near zero cutoff point to avoid random sign flips
constexpr unit DIST_ATTEN(3e-5f);  // light attenuation factor according to object's distance from light source
constexpr unit ZLIMIT(1000.0f);    // max distance of visible objects - camera position
constexpr unit LIGHT_DIST(-75.0f); // light position on Z axis
constexpr unit LIGHT_ELEV(20.0f);  // light position on Y axis
constexpr unit DEG(180.0f);
constexpr unit PI(3.14159265358979323846f);
constexpr unit DEG_TO_RAD = PI / DEG;
constexpr unit NORMAL_SCALE_FACTOR(150.0f);

// Scale factor for EPSILON when computing the squared tolerance
// for detecting nearly-duplicate polygon vertices.
static constexpr unit CONSEC_TOLERANCE_SCALE = 10.0f;

