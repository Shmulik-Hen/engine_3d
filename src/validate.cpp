#include <vector>
#include <cmath>
#include <array>
#include <charconv>

// #define DEBUG_PRINTS
#include "utils.h"
#include "graphics.h"
#include "validate.h"

namespace config_ns
{

using errors_t = std::vector<std::string>;

static void add_error(const std::string& str, errors_t& errs)
{
	errs.push_back(str);
}

static void print_errors(const config_ns::errors_t errs)
{
	PRN(ERR_PFX + ERR_DET);
	for (const auto& err : errs) {
		PRN(TAB + err);
	}
}

static std::string set_precision(const unit val, const uint32_t digits = 3)
{
	std::array<char, 10> buf;

	auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), val, std::chars_format::fixed, digits);

	std::string s(buf.data(), ptr);
	return s;
}

static void check_range_real(const unit val, const unit min, const unit max, const std::string path, errors_t& errs)
{
	if (!std::isfinite(val) || val < min || val > max) {
		// clang-format off
		std::string err = path + set_precision(val) + SPC +
				  ERR_ILG + SPC +
				  LB + set_precision(min) + DASH +
				  set_precision(max) + RB;
		add_error(err, errs);
		// clang-format on
	}
}

static void check_range_uint(const uint32_t val, const uint32_t min, const uint32_t max, const std::string path, errors_t& errs)
{
	if (val < min || val > max) {
		// clang-format off
		std::string err = path + std::to_string(val) + SPC +
				  ERR_ILG+ SPC +
				  LB + std::to_string(min) + DASH +
				  std::to_string(max) + RB;
		add_error(err, errs);
		// clang-format on
	}
}

static void validate_attrib_values(const attrib& att, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	unit val = att.get_rot_x();
	std::string str = path + STR_ROTX + PFX + "   ";
	DBG(str + set_precision(val));
	check_range_real(val, MIN_REAL_VAL, MAX_REAL_VAL, str, errs);
	val = att.get_rot_y();
	str = path + STR_ROTY + PFX + "   ";
	DBG(str + set_precision(val));
	check_range_real(val, MIN_REAL_VAL, MAX_REAL_VAL, str, errs);
	val = att.get_rot_z();
	str = path + STR_ROTZ + PFX + "   ";
	DBG(str + set_precision(val));
	check_range_real(val, MIN_REAL_VAL, MAX_REAL_VAL, str, errs);
	val = att.get_trans_x();
	str = path + STR_TRNX + PFX;
	DBG(str + set_precision(val));
	check_range_real(val, MIN_REAL_VAL, MAX_REAL_VAL, str, errs);
	val = att.get_trans_y();
	str = path + STR_TRNY + PFX;
	DBG(str + set_precision(val));
	check_range_real(val, MIN_REAL_VAL, MAX_REAL_VAL, str, errs);
	val = att.get_trans_z();
	str = path + STR_TRNZ + PFX;
	DBG(str + set_precision(val));
	check_range_real(val, MIN_REAL_VAL, MAX_REAL_VAL, str, errs);
	val = att.get_scale();
	str = path + STR_SCL + PFX + SPC;
	DBG(str + set_precision(val));
	check_range_real(val, MIN_SCALE, MAX_SCALE, str, errs);
}

static void validate_vector_values(const vector_3& vec, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		unit coord = vec.get(i);
		// clang-format off
		std::string str = path + STR_CORD +
				  LB + std::to_string(i + 1) + RB +
				  PFX;
		// clang-format on

		DBG(str + set_precision(coord));
		check_range_real(coord, MIN_REAL_VAL, MAX_REAL_VAL, str, errs);
	}
}

static void validate_polygon_values(const polygon_def& poly, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string name = path + JSON_STR_NAME;
	DBG(name + PFX + QT + poly.name + QT);

	if (poly.name.empty()) {
		add_error(name + SPC + ERR_EMPTY, errs);
	}

	// clang-format off
	std::string color = path + JSON_STR_COLOR + PFX +
			    std::to_string(poly.color_index);
	DBG(color);
	// clang-format on

	// clang-format off
	std::string err = path + JSON_STR_COLOR;
	// clang-format on
	check_range_uint(poly.color_index, 0, valids.max_color_idx, err + PFX, errs);
	// if (poly.color_index > valids.max_color_idx) {
	// 	add_error(color + SPC + ERR_ILG + SPC + LB + std::to_string(valids.max_color_idx) + RB, errs);
	// }

	std::string force = path + JSON_STR_FORCE + PFX + BOOL(poly.force);
	DBG(force);

	// clang-format off
	std::string str = path + STR_NUM + SPC + JSON_STR_VECS;
	DBG(str + PFX + std::to_string(poly.vectors.size()));
	// clang-format on
	check_range_uint(poly.vectors.size(), MIN_VECTORS, MAX_VECTORS, str + PFX, errs);
	int i = 0;
	for (const auto& vector : poly.vectors) {
		// clang-format off
		std::string vec = path + STR_VEC + LB +
				  std::to_string(i + 1) + RB;
		// clang-format on
		// DBG(vec);
		validate_vector_values(vector, vec + PFX, errs, valids);
		i++;
	}
}

static void validate_mesh_values(const mesh_def& mesh, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string msh = path + JSON_STR_NAME;
	DBG(msh + PFX + QT + mesh.name + QT);

	if (mesh.name.empty()) {
		add_error(msh + SPC + ERR_EMPTY, errs);
	}

	DBG(path + STR_NUM + SPC + JSON_STR_POLYS + PFX + std::to_string(mesh.polygons.size()));
	if (mesh.polygons.size() < MIN_POLYGONS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
		                  ERR_ATLST + SPC +
		                  std::to_string(MIN_POLYGONS) +
				  SPC + STR_POLY + SPC + JSON_STR_NAME;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& poly : mesh.polygons) {
			// clang-format off
			std::string pol = path + STR_POLY +
			                  LB + std::to_string(i + 1) + RB;
			// clang-format on
			DBG(pol + PFX + QT + poly + QT);

			if (poly.empty()) {
				std::string err = pol + SPC + ERR_EMPTY;
				add_error(err, errs);
			}
			i++;
		}
	}
}

static void validate_lib_elem_values(const lib_elem_def& elem, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string name = path + JSON_STR_NAME;
	DBG(name + PFX + QT + elem.name + QT);

	if (elem.name.empty()) {
		add_error(name + SPC + ERR_EMPTY, errs);
	}

	std::string mesh = path + JSON_STR_MESH;
	DBG(mesh + PFX + QT + elem.mesh + QT);

	if (elem.mesh.empty()) {
		add_error(mesh + SPC + ERR_EMPTY, errs);
	}
}

static void validate_scene_elem_values(const scene_elem_def& elem, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string name = path + JSON_STR_NAME;
	DBG(name + PFX + QT + elem.name + QT);
	if (elem.name.empty()) {
		add_error(name + SPC + ERR_EMPTY, errs);
	}

	std::string parent = path + JSON_STR_PAREN;
	if (elem.parent.has_value()) {
		DBG(parent + PFX + QT + elem.parent.value() + QT);

		if (elem.parent.value().empty()) {
			add_error(parent + SPC + ERR_EMPTY, errs);
		}
	}
#ifdef DEBUG_PRINTS
	else {
		DBG(parent + SPC + ERR_NULL);
	}
#endif

	DBG(path + JSON_STR_ACT + PFX + BOOL(elem.active));

	std::string ref = path + JSON_STR_REF;
	if (elem.ref.has_value()) {
		DBG(ref + PFX + QT + elem.ref.value() + QT);

		if (elem.ref.value().empty()) {
			add_error(ref + SPC + ERR_EMPTY, errs);
		}
	}

	std::string origin = path + JSON_STR_ORIG;
	if (elem.origin.has_value()) {
		validate_attrib_values(elem.origin.value(), origin + PFX, errs, valids);
	}

	std::string motion = path + JSON_STR_MOTN;
	if (elem.motion.has_value()) {
		validate_attrib_values(elem.motion.value(), motion + PFX, errs, valids);
	}
}

static void validate_environment(const environ_def& env, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	DBG(path);
}

static void validate_runtime(const runtime_def& run, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	DBG(path);
}

static void validate_library(const library_def& lib, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	DBG(path + STR_NUM + SPC + JSON_STR_POLYS + PFX + std::to_string(lib.polygons.size()));
	if (lib.polygons.size() < MIN_POLYGONS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
				  std::to_string(MIN_POLYGONS) +
				  SPC + STR_POLY;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& polygon : lib.polygons) {
			// clang-format off
			std::string polygons = path + STR_POLY +
					       LB + std::to_string(i + 1) + RB;
			// clang-format on
			validate_polygon_values(polygon, polygons + PFX, errs, valids);
			i++;
		}
	}

	DBG(path + STR_NUM + SPC + JSON_STR_MESHS + PFX + std::to_string(lib.meshes.size()));
	if (lib.meshes.size() < MIN_MESHES) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_MESHES) +
				  SPC + JSON_STR_MESH;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& mesh : lib.meshes) {
			// clang-format off
			std::string meshes = path + JSON_STR_MESH +
					     LB + std::to_string(i + 1) + RB;
			// clang-format on
			validate_mesh_values(mesh, meshes + PFX, errs, valids);
			i++;
		}
	}

	DBG(path + STR_NUM + SPC + JSON_STR_ELEMS + PFX + std::to_string(lib.lib_elem_defs.size()));
	if (lib.lib_elem_defs.size() < MIN_LIB_ELEMS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_LIB_ELEMS) +
				  SPC + STR_ELEM;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& element : lib.lib_elem_defs) {
			// clang-format off
			std::string elems = path + STR_ELEM +
					    LB + std::to_string(i + 1) + RB;
			// clang-format on
			validate_lib_elem_values(element, elems + PFX, errs, valids);
			i++;
		}
	}
}

static void validate_scene(const scene_def& scene, const std::string path, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	std::string name = path + JSON_STR_NAME;
	DBG(name + PFX + QT + scene.name + QT);

	if (scene.name.empty()) {
		add_error(name + SPC + ERR_EMPTY, errs);
	}

	DBG(path + STR_NUM + SPC + JSON_STR_ELEMS + PFX + std::to_string(scene.scene_elem_defs.size()));
	if (scene.scene_elem_defs.size() < MIN_SCN_ELEMS) {
		// clang-format off
		std::string err = path + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
		                  std::to_string(MIN_SCN_ELEMS) +
				  SPC + JSON_STR_ELEMS;
		// clang-format on
		add_error(err, errs);
	}

	int i = 0;
	for (const auto& elem : scene.scene_elem_defs) {
		// clang-format off
		std::string elems = path + STR_ELEM +
				    LB + std::to_string(i + 1) + RB;
		// clang-format on
		validate_scene_elem_values(elem, elems + PFX, errs, valids);
		i++;
	}

	if (scene.env.has_value()) {
		std::string env = path + JSON_STR_ENV;
		// DBG(env);
		validate_environment(scene.env.value(), env + PFX, errs, valids);
	}

	if (scene.run.has_value()) {
		std::string run = path + STR_RUN;
		// DBG(run);
		validate_runtime(scene.run.value(), run + PFX, errs, valids);
	}
}

static void validate_values(const config_ns::AST& ast, const std::string path, errors_t& errs)
{
	std::string local_path = path + STR_VALS;

	DBG(local_path);

	std::string lib = local_path + PFX + JSON_STR_LIB;
	DBG(lib);

	validate_library(ast.library_defs, lib + PFX, errs, ast.valids);

	std::string scenes = local_path + PFX + JSON_STR_SCNS;
	DBG(scenes);

	// clang-format off
	DBG(scenes + PFX + STR_NUM + SPC + JSON_STR_SCNS + PFX +
	    std::to_string(ast.scene_defs.size()));
	// clang-format on

	if (ast.scene_defs.size() < MIN_SCENES) {
		// clang-format off
		std::string err = scenes + SPC + ERR_REQ + SPC +
				  ERR_ATLST + SPC +
				  std::to_string(MIN_SCENES) +
				  SPC + STR_SCN;
		// clang-format on
		add_error(err, errs);
	}
	else {
		int i = 0;
		for (const auto& scene : ast.scene_defs) {
			// clang-format off
			std::string scn = scenes + PFX + STR_SCN +
					  LB + std::to_string(i + 1) + RB;
			// clang-format on
			validate_scene(scene, scn + PFX, errs, ast.valids);
			i++;
		}
	}
}

static void validate_references(const config_ns::AST& ast, const std::string path, errors_t& errs)
{
	DBG(path + STR_REFS);

	std::string local_path = path + STR_REFS + PFX;
}

static void validate_hierarchy(const config_ns::AST& ast, const std::string path, errors_t& errs)
{
	DBG(path + STR_HIER);

	std::string local_path = path + STR_HIER;
}

bool validate_ast(const config_ns::AST& ast)
{
	DBG(STR_VAL);

	config_ns::errors_t errs;
	std::string path = STR_VAL + PFX;

	validate_values(ast, path, errs);
	if (errs.size()) {
		print_errors(errs);
		return false;
	}

	validate_references(ast, path, errs);
	if (errs.size()) {
		print_errors(errs);
		return false;
	}

	validate_hierarchy(ast, path, errs);
	if (errs.size()) {
		print_errors(errs);
		return false;
	}

	return true;
}

} // namespace config_ns
