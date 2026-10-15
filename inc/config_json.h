#pragma once

#include <fstream>
#include <json/json.h>
#include "config_document.h"

namespace config_ns
{

// supported field names
// clang-format off
const std::string JSON_STR_VER   = "version";
const std::string JSON_STR_NAME  = "name";
const std::string JSON_STR_LIB   = "library";
const std::string JSON_STR_POLYS = "polygons";
const std::string JSON_STR_COLOR = "color_idx";
const std::string JSON_STR_FORCE = "force";
const std::string JSON_STR_VECS  = "vectors";
const std::string JSON_STR_MESHS = "meshes";
const std::string JSON_STR_ELEMS = "elements";
const std::string JSON_STR_MESH  = "mesh";
const std::string JSON_STR_SCNS  = "scenes";
const std::string JSON_STR_PAREN = "parent";
const std::string JSON_STR_ACT   = "active";
const std::string JSON_STR_REF   = "ref";
const std::string JSON_STR_ATTR  = "attrib";
const std::string JSON_STR_MOTN  = "motion";
const std::string JSON_STR_ENV   = "environment";
// environment field names
const std::string JSON_STR_LIGHT_TYPE = "light_type";
const std::string JSON_STR_LIGHT_DIR  = "light_direction";
const std::string JSON_STR_LIGHT_POS  = "light_position";
const std::string JSON_STR_CAM_POS    = "camera_position";
const std::string JSON_STR_FOCAL_LEN  = "focal_len";
const std::string JSON_STR_NEAR_EPS   = "near_eps";
const std::string JSON_STR_MIN_POS    = "min_pos";
const std::string JSON_STR_MAX_POS    = "max_pos";
const std::string JSON_STR_LOOPS      = "loops";
const std::string JSON_STR_LOOP_DELAY = "loop_delay";
// clang-format on

AST parse_json(const std::string&);

} // namespace config_ns
