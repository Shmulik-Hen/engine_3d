#pragma once

#include <cstdint>
#include <limits>

#include "unit.h"

// versions
const uint32_t APP_VER_MAJ(0U);
const uint32_t APP_VER_MIN(10U);
const uint32_t APP_VER_REV(0U);
const uint32_t JSON_VER_MAJ(0U);
const uint32_t JSON_VER_MIN(0U);
const uint32_t JSON_VER_REV(3U);
const uint32_t JSON_VER_FIELDS(3U);

// coordinates
const uint32_t X_(0U);
const uint32_t Y_(1U);
const uint32_t Z_(2U);
const uint32_t O_(3U);

// array sizes
const uint32_t MIN_VECTORS(3U);
const uint32_t MAX_VECTORS(8U);
const uint32_t NUM_POINTS(2U);
const uint32_t NUM_ATTRIBUTES(7U);
const uint32_t NUM_COORDS(O_);

// const values
constexpr unit DEG(180.0f);
constexpr unit PI(3.14159265358979323846f);
constexpr unit DEG_TO_RAD(PI / DEG);

// default values
const unit UNIT(1.0f);        // a '1' in engines units
const unit ZERO(0.0f);        // a '0' in engines units
const unit EPSILON(1e-5f);    // near zero cutoff point to avoid random sign flips
const unit DIST_ATTEN(3e-5f); // light attenuation factor according to object's distance from light source
const unit LIGHT_DIR_X(ZERO);
const unit LIGHT_DIR_Y(ZERO);
const unit LIGHT_DIR_Z(UNIT);
const unit LIGHT_POS_X(ZERO);
const unit LIGHT_POS_Y(20.0f);
const unit LIGHT_POS_Z(75.0f);
const unit CAM_POS_X(ZERO);
const unit CAM_POS_Y(ZERO);
const unit CAM_POS_Z(1000.0f);
const unit FOCAL_LEN(2000.0f);
const unit NEAR_EPS(UNIT);
const unit NORMAL_SCALE_FACTOR(50.0f);
// Scale factor for EPSILON when computing the squared tolerance
// for detecting nearly-duplicate polygon vectors.
const unit CONSEC_TOLERANCE_SCALE(10.0f);
const uint32_t DEF_LOOP_DELAY(20U);
const int FORWARD_LOOKING_SIGN(-1);
const int DEF_LOOP_NUM(-1); // endless
