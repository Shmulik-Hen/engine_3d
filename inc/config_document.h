#pragma once

#include <cstdint>
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
	std::string name;
	int color_index {0};
	bool force {false};

	std::vector<vector_3> points;

	// keep for transition; later we can drop it
	std::optional<vector_3> normal_cfg;
};

struct element_def
{
	std::string name;
	std::string parent; // empty or "world" etc.
	attrib att;
	bool active {false};

	// refer to polygons by name (stable for both legacy and JSON)
	std::vector<std::string> polygons;
};

struct document
{
	std::vector<polygon_def> polygons;
	std::vector<element_def> elements;
};

} // namespace config_ns
