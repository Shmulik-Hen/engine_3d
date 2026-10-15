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
	std::vector<vector_3> vertices;
};

struct mesh_def
{
	// required fields
	std::string name;
	std::vector<std::string> polygons;
};

struct element_def
{
	// required fields
	std::string name;
	std::string mesh;
};
struct element_ref
{
	// required fields
	std::string name;
	bool active;

	// optional fields
	std::optional<std::string> ref;
	std::optional<std::string> parent; // null means "root"
	std::optional<attrib> ini_att;
	std::optional<attrib> run_att;
};

struct point_def
{
	// required fields
	uint32_t x;
	uint32_t y;
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

struct scene_def
{
	std::string name;
	environ_def env;
	runtime_def run;
};

struct AST
{
	std::vector<polygon_def> polygons;
	std::vector<mesh_def> meshes;
	std::vector<element_def> element_defs;
	std::vector<element_ref> element_refs;
	std::vector<scene_def> scene_defs;
};

} // namespace config_ns
