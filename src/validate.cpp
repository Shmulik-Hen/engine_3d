#include <vector>

#define DEBUG_PRINTS
#include "utils.h"
#include "graphics.h"
#include "validate.h"

namespace config_ns
{

using errors_t = std::vector<std::string>;

// clang-format off
constexpr std::string_view ERR_EMPTY = "is empty";
constexpr std::string_view ERR_ILG   = "is illegal";
constexpr std::string_view ERR_NULL  = "is null";

#ifdef DEBUG_PRINTS
constexpr std::string_view DBG_VAL_AST   = "validate ast";
constexpr std::string_view DBG_VAL_VALS  = "validate values";
constexpr std::string_view DBG_VAL_LIB   = "validate library";
constexpr std::string_view DBG_VAL_POLY  = "validate polygon";
constexpr std::string_view DBG_POLY_VALS = "  polygon values";
constexpr std::string_view DBG_VAL_VEC   = "    validate vector";
constexpr std::string_view DBG_VEC_VALS  = "      vector values";
constexpr std::string_view DBG_VAL_MESH  = "validate mesh";
constexpr std::string_view DBG_MESH_VALS = "  mesh values";
constexpr std::string_view DBG_VAL_EDEF  = "validate element";
constexpr std::string_view DBG_EDEF_VALS = "  element values";
constexpr std::string_view DBG_VAL_SCNS  = "validate scenes";
constexpr std::string_view DBG_VAL_SCN   = "validate scene";
constexpr std::string_view DBG_SCN_VALS  = "  scene values";
constexpr std::string_view DBG_VAL_EREF  = "    validate scene element";
constexpr std::string_view DBG_EREF_VALS = "      scene element values";
constexpr std::string_view DBG_VAL_ATT   = "        validate attrib";
constexpr std::string_view DBG_ATT_VALS  = "          attrib values";
constexpr std::string_view DBG_VAL_HIER  = "  validate hierarchy";
constexpr std::string_view DBG_VAL_REFS  = "  validate references";
#endif // DEBUG_PRINTS
// clang-format on

static void add_error(const std::string& str, errors_t& errs)
{
	errs.push_back(str);
}

static void validate_attrib_values(const attrib& att, const std::string name, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
	    DBG_VAL_ATT + LB + name + RB + PFX);
	// clang-format on

	unit val = att.get_rot_x();

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN +
	    PFX + DBG_ATT_VALS + PFX + JSON_STR_ROTX +
	    PFX + std::to_string(val));
	// clang-format on

	if (val < MIN_REAL_VAL || val > MAX_REAL_VAL) {
		// clang-format off
		add_error(JSON_STR_SCN + PFX + JSON_STR_ROTX + SPC +
			  ERR_ILG + PFX + std::to_string(val) +
			  SPC + LB + std::to_string(MIN_REAL_VAL) +
			  DASH + std::to_string(MAX_REAL_VAL) + RB, errs);
		// clang-format on
	}

	val = att.get_rot_y();

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN +
	    PFX + DBG_ATT_VALS + PFX + JSON_STR_ROTY +
	    PFX + std::to_string(val));
	// clang-format on

	if (val < MIN_REAL_VAL || val > MAX_REAL_VAL) {
		// clang-format off
		add_error(JSON_STR_SCN + PFX + JSON_STR_ROTY + SPC +
			  ERR_ILG + PFX + std::to_string(val) +
			  SPC + LB + std::to_string(MIN_REAL_VAL) +
			  DASH + std::to_string(MAX_REAL_VAL) + RB, errs);
		// clang-format on
	}

	val = att.get_rot_z();

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN +
	    PFX + DBG_ATT_VALS + PFX + JSON_STR_ROTZ +
	    PFX + std::to_string(val));
	// clang-format on

	if (val < MIN_REAL_VAL || val > MAX_REAL_VAL) {
		// clang-format off
		add_error(JSON_STR_SCN + PFX + JSON_STR_ROTZ + SPC +
			  ERR_ILG + PFX + std::to_string(val) +
			  SPC + LB + std::to_string(MIN_REAL_VAL) +
			  DASH + std::to_string(MAX_REAL_VAL) + RB, errs);
		// clang-format on
	}

	val = att.get_trans_x();

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN +
	    PFX + DBG_ATT_VALS + PFX + JSON_STR_TRNX +
	    PFX + std::to_string(val));
	// clang-format on

	if (val < MIN_REAL_VAL || val > MAX_REAL_VAL) {
		// clang-format off
		add_error(JSON_STR_SCN + PFX + JSON_STR_TRNX + SPC +
			  ERR_ILG + PFX + std::to_string(val) +
			  SPC + LB + std::to_string(MIN_REAL_VAL) +
			  DASH + std::to_string(MAX_REAL_VAL) + RB, errs);
		// clang-format on
	}

	val = att.get_trans_y();

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN +
	    PFX + DBG_ATT_VALS + PFX + JSON_STR_TRNY +
	    PFX + std::to_string(val));
	// clang-format on

	if (val < MIN_REAL_VAL || val > MAX_REAL_VAL) {
		// clang-format off
		add_error(JSON_STR_SCN + PFX + JSON_STR_TRNY + SPC +
			  ERR_ILG + PFX + std::to_string(val) +
			  SPC + LB + std::to_string(MIN_REAL_VAL) +
			  DASH + std::to_string(MAX_REAL_VAL) + RB, errs);
		// clang-format on
	}

	val = att.get_trans_z();

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN +
	    PFX + DBG_ATT_VALS + PFX + JSON_STR_TRNZ +
	    PFX + std::to_string(val));
	// clang-format on

	if (val < MIN_REAL_VAL || val > MAX_REAL_VAL) {
		// clang-format off
		add_error(JSON_STR_SCN + PFX + JSON_STR_TRNZ + SPC +
			  ERR_ILG + PFX + std::to_string(val) +
			  SPC + LB + std::to_string(MIN_REAL_VAL) +
			  DASH + std::to_string(MAX_REAL_VAL) + RB, errs);
		// clang-format on
	}

	val = att.get_scale();

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN +
	    PFX + DBG_ATT_VALS + PFX + JSON_STR_SCL +
	    PFX + std::to_string(val));
	// clang-format on

	if (val < MIN_SCALE || val > MAX_SCALE) {
		// clang-format off
		add_error(JSON_STR_SCN + PFX + JSON_STR_SCL + SPC +
			  ERR_ILG + PFX + std::to_string(val) +
			  SPC + LB + std::to_string(MIN_SCALE) +
			  DASH + std::to_string(MAX_SCALE) + RB, errs);
		// clang-format on
	}
}

static void validate_vector_values(const vector_3& vec, const int idx, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_VAL_VEC + LB + std::to_string(idx+1) + RB + PFX);
	// clang-format on

	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		unit coord = vec.get(i);

		// clang-format off
		DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB +
		    PFX + DBG_VEC_VALS + PFX + JSON_STR_VEC +
		    LB + std::to_string(i+1) + RB + PFX +
		    std::to_string(coord));
		// clang-format on

		if (coord < MIN_REAL_VAL || coord > MAX_REAL_VAL) {
			// clang-format off
			add_error(JSON_STR_LIB + PFX + JSON_STR_VEC +
				  LB + std::to_string(idx+1) + RB +
				  LB + std::to_string(i+1) + RB + PFX +
				  ERR_ILG + PFX + std::to_string(coord) +
				  SPC + LB + std::to_string(MIN_REAL_VAL) +
				  DASH + std::to_string(MAX_REAL_VAL) + RB, errs);
			// clang-format on
		}
	}
}

static void validate_polygon_values(const polygon_def& poly, const int idx, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_VAL_POLY + LB + std::to_string(idx+1) + RB + PFX);

	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_POLY_VALS + PFX + JSON_STR_NAME + PFX +
	    QT + poly.name + QT);
	// clang-format on

	if (poly.name.empty()) {
		// clang-format off
		add_error(JSON_STR_LIB + PFX + JSON_STR_POLY +
			  LB + std::to_string(idx) + RB + PFX +
			  JSON_STR_NAME + SPC + ERR_EMPTY, errs);
		// clang-format on
	}

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_POLY_VALS + PFX + JSON_STR_COLOR + PFX +
	    std::to_string(poly.color_index));
	// clang-format on

	if (poly.color_index > valids.max_colors) {
		// clang-format off
		add_error(JSON_STR_LIB + PFX + JSON_STR_POLY +
			  LB + std::to_string(idx) + RB + PFX +
			  JSON_STR_COLOR + SPC + ERR_ILG, errs);
		// clang-format on
	}

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_POLY_VALS + PFX + JSON_STR_FORCE + PFX +
	    BOOL(poly.force));
	// clang-format on

	int i = 0;
	for (const auto& vector : poly.vectors) {
		validate_vector_values(vector, i, errs, valids);
		i++;
	}
}

static void validate_mesh_values(const mesh_def& mesh, const int idx, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_VAL_MESH + LB + std::to_string(idx+1) + RB + PFX);

	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_MESH_VALS + PFX + JSON_STR_NAME + PFX +
	    QT + mesh.name + QT);
	// clang-format on

	if (mesh.name.empty()) {
		// clang-format off
		add_error(JSON_STR_LIB + PFX + JSON_STR_MESH +
			  LB + std::to_string(idx) + RB + PFX +
			  JSON_STR_NAME + SPC + ERR_EMPTY, errs);
		// clang-format on
	}

	int i = 0;
	for (const auto& poly : mesh.polygons) {
		// clang-format off
		DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB +
		    PFX + DBG_MESH_VALS + PFX + JSON_STR_POLYS +
		    LB + std::to_string(i) + RB + PFX + QT + poly + QT);
		// clang-format on

		if (poly.empty()) {
			// clang-format off
			add_error(JSON_STR_LIB + PFX + JSON_STR_MESH +
				  LB + std::to_string(idx) + RB + PFX +
				  JSON_STR_POLYS + LB + std::to_string(i) + RB +
				  SPC + ERR_EMPTY, errs);
			// clang-format on
		}
		i++;
	}
}

static void validate_elem_def_values(const lib_elem_def& elem, const int idx, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_VAL_EDEF + LB + std::to_string(idx+1) + RB + PFX);

	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_EDEF_VALS + PFX + JSON_STR_NAME + PFX +
	    QT + elem.name + QT);
	// clang-format on

	if (elem.name.empty()) {
		// clang-format off
		add_error(JSON_STR_LIB + PFX + JSON_STR_ELEM +
			  LB + std::to_string(idx) + RB + PFX +
			  JSON_STR_NAME + SPC + ERR_EMPTY, errs);
		// clang-format on
	}

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_LIB + PFX +
	    DBG_EDEF_VALS + PFX + JSON_STR_MESH + PFX +
	    QT + elem.name + QT);
	// clang-format on

	if (elem.mesh.empty()) {
		// clang-format off
		add_error(JSON_STR_LIB + PFX + JSON_STR_ELEM +
			  LB + std::to_string(idx) + RB + PFX +
			  JSON_STR_MESH + SPC + ERR_EMPTY, errs);
		// clang-format on
	}
}

static void validate_library(const AST& ast, errors_t& errs)
{
	DBG(STR_VAL + PFX + TAB + TAB + DBG_VAL_LIB + PFX);

	int i = 0;
	for (const auto& polygon : ast.library_defs.polygons) {
		validate_polygon_values(polygon, i, errs, ast.valids);
		i++;
	}

	i = 0;
	for (const auto& mesh : ast.library_defs.meshes) {
		validate_mesh_values(mesh, i, errs, ast.valids);
		i++;
	}

	i = 0;
	for (const auto& element : ast.library_defs.lib_elem_defs) {
		validate_elem_def_values(element, i, errs, ast.valids);
		i++;
	}
}

static void validate_elem_ref_values(const scene_elem_def& elem, const int idx, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
	    DBG_VAL_EREF + LB + std::to_string(idx+1) + RB + PFX);

	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
	    DBG_EREF_VALS + PFX + JSON_STR_NAME + PFX +
	    QT + elem.name + QT);
	// clang-format on

	if (elem.name.empty()) {
		// clang-format off
		add_error(JSON_STR_SCN + PFX + JSON_STR_ELEM +
			  LB + std::to_string(idx) + RB + PFX +
			  JSON_STR_NAME + SPC + ERR_EMPTY, errs);
		// clang-format on
	}

	if (elem.parent.has_value()) {
		// clang-format off
		DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
		    DBG_EREF_VALS + PFX + JSON_STR_PAREN + PFX +
		    QT + elem.parent.value() + QT);
		// clang-format on

		if (elem.parent.value().empty()) {
			// clang-format off
			add_error(JSON_STR_SCN + PFX + JSON_STR_ELEM +
				  LB + std::to_string(idx) + RB + PFX +
				  JSON_STR_PAREN + SPC + ERR_EMPTY, errs);
			// clang-format on
		}
	}
#ifdef DEBUG_PRINTS
	else {
		// clang-format off
		DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
		    DBG_EREF_VALS + PFX + JSON_STR_PAREN + SPC + ERR_NULL);
		// clang-format on
	}
#endif

	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
	    DBG_EREF_VALS + PFX + JSON_STR_ACT + PFX + BOOL(elem.active));
	// clang-format on

	if (elem.ref.has_value()) {
		// clang-format off
		DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
		    DBG_EREF_VALS + PFX + JSON_STR_REF + PFX +
		    QT + elem.ref.value() + QT);
		// clang-format on

		if (elem.ref.value().empty()) {
			// clang-format off
			add_error(JSON_STR_SCN + PFX + JSON_STR_ELEM +
				  LB + std::to_string(idx) + RB + PFX +
				  JSON_STR_REF + SPC + ERR_EMPTY, errs);
			// clang-format on
		}
	}

	if (elem.origin.has_value()) {
		validate_attrib_values(elem.origin.value(), JSON_STR_ORIG, errs, valids);
	}

	if (elem.motion.has_value()) {
		validate_attrib_values(elem.motion.value(), JSON_STR_MOTN, errs, valids);
	}
}

static void validate_scene(const scene_def& scene, const int idx, errors_t& errs, const validate_def valids [[maybe_unused]])
{
	// clang-format off
	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
	    DBG_VAL_SCN + LB + std::to_string(idx+1) + RB + PFX);

	DBG(STR_VAL + PFX + TAB + TAB + JSON_STR_SCN + PFX +
	    DBG_SCN_VALS + PFX + JSON_STR_NAME + PFX +
	    QT + scene.name + QT);
	// clang-format on

	if (scene.name.empty()) {
		// clang-format off
		add_error(JSON_STR_SCN + LB + std::to_string(idx) + RB +
			  PFX + JSON_STR_NAME + SPC + ERR_EMPTY, errs);
		// clang-format on
	}

	int i = 0;
	for (const auto& elem : scene.scene_elem_defs) {
		validate_elem_ref_values(elem, i, errs, valids);
		i++;
	}
}

static void validate_values(const config_ns::AST& ast, errors_t& errs)
{
	DBG(STR_VAL + PFX + TAB + DBG_VAL_VALS + PFX);

	validate_library(ast, errs);

	DBG(STR_VAL + PFX + TAB + TAB + DBG_VAL_SCNS + PFX);

	int i = 0;
	for (const auto& scene : ast.scene_defs) {
		validate_scene(scene, i, errs, ast.valids);
		i++;
	}
}

static void validate_references(const config_ns::AST& ast, errors_t& errs)
{
	DBG(STR_VAL + PFX + DBG_VAL_REFS + PFX);
}

static void validate_hierarchy(const config_ns::AST& ast, errors_t& errs)
{
	DBG(STR_VAL + PFX + DBG_VAL_HIER + PFX);
}

bool validate_ast(const config_ns::AST& ast)
{
	config_ns::errors_t errs;

	DBG(STR_VAL + PFX + DBG_VAL_AST + PFX);

	validate_values(ast, errs);

	validate_references(ast, errs);

	validate_hierarchy(ast, errs);

	if (errs.size()) {
		PRN(ERR_PFX + STR_VAL + PFX + "errors detected" + PFX);
		for (const auto& err : errs) {
			PRN(err);
		}
		return false;
	}

	return true;
}

} // namespace config_ns
