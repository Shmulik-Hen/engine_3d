#pragma once

#include <string>
#include <vector>
#include <optional>
#include <string_view>

#include "attrib.h"
#include "vector_3.h"

namespace config_ns
{

using vector_3 = vector_3_ns::vector_3;
using attrib = attrib_ns::attrib;

// clang-format off
// supported field names
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
const std::string JSON_STR_ORIG  = "origin";
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
// debug/error messages
constexpr std::string_view STR_PRS       = "parse";
constexpr std::string_view STR_VAL       = "validate";
constexpr std::string_view JSON_STR_SCN  = "scene";
constexpr std::string_view JSON_STR_ATT  = "attrib";
constexpr std::string_view JSON_STR_VEC  = "vector";
constexpr std::string_view JSON_STR_PAIR = "pair";
constexpr std::string_view JSON_STR_POLY = "polygon";
constexpr std::string_view JSON_STR_ELEM = "element";
constexpr std::string_view JSON_STR_ROTX = "X rotation   ";
constexpr std::string_view JSON_STR_ROTY = "Y rotation   ";
constexpr std::string_view JSON_STR_ROTZ = "Z rotation   ";
constexpr std::string_view JSON_STR_TRNX = "X translation";
constexpr std::string_view JSON_STR_TRNY = "Y translation";
constexpr std::string_view JSON_STR_TRNZ = "Z translation";
constexpr std::string_view JSON_STR_SCL  = "scale factor ";
constexpr std::string_view ERR_VER       = " unsupported version";
constexpr std::string_view ERR_REQ       = "required";
constexpr std::string_view ERR_CURR      = "current";
constexpr std::string_view ERR_OBJ       = "not an object";
constexpr std::string_view ERR_ARR       = "not an array";
constexpr std::string_view ERR_UINT      = "not an unsigned integer";
constexpr std::string_view ERR_INT       = "not an integer";
constexpr std::string_view ERR_STR       = "not a string";
constexpr std::string_view ERR_FLOAT     = "not a floating point";
constexpr std::string_view ERR_BOOL      = "not a boolean";
constexpr std::string_view ERR_SIZE      = "illegal array size";
constexpr std::string_view ERR_FAIL      = "failed";
#ifdef DEBUG_PRINTS
constexpr std::string_view DBG_SIZE  = "size";
constexpr std::string_view DBG_FOUND = "found";
#endif // DEBUG_PRINTS
// clang-format on

struct polygon_def
{
	// required fields
	std::string name;
	uint32_t color_index {0};
	bool force {false};
	std::vector<vector_3> vectors;
};

struct mesh_def
{
	// required fields
	std::string name;
	std::vector<std::string> polygons;
};

struct lib_elem_def
{
	// required fields
	std::string name;
	std::string mesh;
};

struct scene_elem_def
{
	// required fields
	std::string name;
	bool active {false};

	// optional fields
	std::optional<std::string> ref;
	std::optional<std::string> parent; // null means "root"
	std::optional<attrib> origin;
	std::optional<attrib> motion;
};

// merged during build from lib_elem_def + scene_elem_def
struct element_def
{
	// required fields
	std::string name;
	std::string parent; // null means "root"
	std::optional<attrib> origin;
	std::optional<attrib> motion;
	bool active {false};

	// refer to polygons by name (stable for both legacy and JSON)
	std::vector<std::string> polygons;
};

struct point_def
{
	// required fields
	uint32_t x {0};
	uint32_t y {0};
};

struct runtime_def
{
	// optional fields
	std::optional<int> loops;
	std::optional<uint32_t> loop_delay;
};

struct environ_def
{
	// optional fields
	std::optional<uint32_t> color_idx;
	std::optional<uint32_t> light_type;
	std::optional<vector_3> light_direction;
	std::optional<vector_3> light_position;
	std::optional<vector_3> camera_position;
	std::optional<unit> focal_len;
	std::optional<unit> near_eps;
	std::optional<point_def> min_pos;
	std::optional<point_def> max_pos;
};

struct library_def
{
	std::vector<polygon_def> polygons;
	std::vector<mesh_def> meshes;
	std::vector<lib_elem_def> lib_elem_defs;
};

struct scene_def
{
	std::string name;
	environ_def env;
	runtime_def run;
	std::vector<scene_elem_def> scene_elem_defs;
};

struct validate_def
{
	uint32_t max_colors {0};
};

struct AST
{
	library_def library_defs;
	std::vector<scene_def> scene_defs;
	validate_def valids;
};

} // namespace config_ns
