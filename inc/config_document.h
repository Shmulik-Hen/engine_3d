#pragma once

#include <string>
#include <vector>
#include <optional>

#include "attrib.h"
#include "vector_3.h"

namespace config_ns
{

using vector_3 = vector_3_ns::vector_3;
using attrib = attrib_ns::attrib;

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

struct AST
{
	library_def library_defs;
	std::vector<scene_def> scene_defs;
};

} // namespace config_ns
