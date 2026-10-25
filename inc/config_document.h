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
// supported field names - jsoncpp uses only std::string
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
const std::string JSON_STR_RES        = "resolution";
const std::string JSON_STR_LOOPS      = "loops";
const std::string JSON_STR_LOOP_DELAY = "loop_delay";
// error/debug messages
constexpr std::string_view STR_NUM  = "num";
constexpr std::string_view STR_RNTM = "run_time";
constexpr std::string_view STR_PRS  = "parse";
constexpr std::string_view STR_VAL  = "validate";
constexpr std::string_view STR_SCN  = "scene";
constexpr std::string_view STR_ATT  = "attrib";
constexpr std::string_view STR_VEC  = "vector";
constexpr std::string_view STR_PAIR = "pair";
constexpr std::string_view STR_ARR  = "array";
constexpr std::string_view STR_POLY = "polygon";
constexpr std::string_view STR_ELEM = "element";
constexpr std::string_view STR_ROTX = "rotation X";
constexpr std::string_view STR_ROTY = "rotation Y";
constexpr std::string_view STR_ROTZ = "rotation Z";
constexpr std::string_view STR_TRNX = "translation X";
constexpr std::string_view STR_TRNY = "translation Y";
constexpr std::string_view STR_TRNZ = "translation Z";
constexpr std::string_view STR_SCL  = "scale factor";
constexpr std::string_view STR_AST  = "ast";
constexpr std::string_view STR_VLDT = "validate";
constexpr std::string_view STR_VALS = "values";
constexpr std::string_view STR_REFS = "references";
constexpr std::string_view STR_HIER = "hierarchy";
constexpr std::string_view STR_WID  = "width";
constexpr std::string_view STR_HGT  = "height";
constexpr std::string_view STR_CORD = "coord";
constexpr std::string_view ERR_VER  = " unsupported version";
constexpr std::string_view ERR_REQ  = "requires";
constexpr std::string_view ERR_CURR = "current";
constexpr std::string_view ERR_OBJ  = "not an object";
constexpr std::string_view ERR_ARR  = "not an array";
constexpr std::string_view ERR_UINT = "not an unsigned integer";
constexpr std::string_view ERR_INT  = "not an integer";
constexpr std::string_view ERR_STR  = "not a string";
constexpr std::string_view ERR_REAL = "not a floating point";
constexpr std::string_view ERR_BOOL = "not a boolean";
constexpr std::string_view ERR_SIZE = "illegal array size";
constexpr std::string_view ERR_FAIL = "failed";
constexpr std::string_view ERR_DET  = "errors detected !";
constexpr std::string_view ERR_MISS = "is missing";
constexpr std::string_view ERR_ILG  = "is illegal";
constexpr std::string_view ERR_NULL = "is null";
constexpr std::string_view ERR_ATLST = "at least";
constexpr std::string_view ERR_EMPTY = "is empty";
// clang-format on

enum light_type
{
	__lt_min__,               // dummy
	directional = __lt_min__, // sun-like
	point,                    // light-bulb
	__lt_max__                // dummy
};

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
	bool active {false};
	std::vector<std::string> polygons;

	// optional fields
	std::string parent; // null means "root"
	std::optional<attrib> origin;
	std::optional<attrib> motion;
};

struct point_def
{
	// required fields
	uint32_t x {0};
	uint32_t y {0};
};

struct resolution_def
{
	// required fields
	uint32_t width {0};
	uint32_t height {0};
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
	std::optional<resolution_def> resolution;
};

struct library_def
{
	// required fields
	std::vector<polygon_def> polygons;
	std::vector<mesh_def> meshes;
	std::vector<lib_elem_def> lib_elem_defs;
};

struct scene_def
{
	// required fields
	std::string name;
	std::vector<scene_elem_def> scene_elem_defs;

	// optional fields
	std::optional<environ_def> env;
	std::optional<runtime_def> run;
};

struct validate_def
{
	uint32_t max_color_idx {0};
};

struct AST
{
	library_def library_defs;
	std::vector<scene_def> scene_defs;
	validate_def valids;
};

} // namespace config_ns
