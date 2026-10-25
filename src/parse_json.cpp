#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

// #define DEBUG_PRINTS
#include "utils.h"
#include "parse_json.h"

namespace config_ns
{

// clang-format off
#ifdef DEBUG_PRINTS
constexpr std::string_view DBG_VER   = "check_ver";
constexpr std::string_view DBG_ATT   = "parse_attrib";
constexpr std::string_view DBG_VEC   = "parse_vector";
constexpr std::string_view DBG_PAIR  = "parse_pair";
constexpr std::string_view DBG_POLY  = "parse_polygon";
constexpr std::string_view DBG_MESH  = "parse_mesh";
constexpr std::string_view DBG_EDEF  = "parse_elem_def";
constexpr std::string_view DBG_EREF  = "parse_elem_ref";
constexpr std::string_view DBG_LIB   = "parse_lib";
constexpr std::string_view DBG_ENV   = "parse_env";
constexpr std::string_view DBG_SCN   = "parse_scene";
constexpr std::string_view DBG_JSON  = "parse_json";
constexpr std::string_view DBG_NUM   = "final num";
constexpr std::string_view DBG_SIZE  = "size";
constexpr std::string_view DBG_FOUND = "found";
#endif // DEBUG_PRINTS
// clang-format on

static bool check_json_version(const Json::Value& ver, const uint32_t array[JSON_VER_FIELDS])
{
	if (!ver.isArray()) {
		ERR(STR_PRS << PFX << JSON_STR_VER << PFX << ERR_ARR);
		return false;
	}

	if (ver.size() != JSON_VER_FIELDS) {
		ERR(STR_PRS << PFX << JSON_STR_VER << PFX << ERR_SIZE << PFX
		            << ERR_REQ << PFX << JSON_VER_FIELDS << SEP
		            << ERR_CURR << PFX << ver.size());
		return false;
	}

	bool match = true;
	for (uint32_t i = 0; i < JSON_VER_FIELDS; i++) {
		if (!ver[i].isUInt()) {
			ERR(STR_PRS << PFX << JSON_STR_VER << LB << (i + 1) << RB
			            << PFX << ERR_UINT);
			return false;
		}

		if (ver[i].asUInt() != array[i]) {
			match = false;
			break;
		}
	}

	if (!match) {
		ERR(STR_PRS << PFX << JSON_STR_VER << PFX << ERR_VER
		            << PFX << ERR_REQ << PFX
		            << array[0] << DOT << array[1] << DOT << array[2]
		            << SEP << ERR_CURR << PFX
		            << ver[0] << DOT << ver[1] << DOT << ver[2]);
		return false;
	}

	DBG(STR_PRS << PFX << DBG_VER << PFX << "found version" << PFX
	            << array[0] << DOT << array[1] << DOT << array[2]);

	return true;
}

static bool parse_json_attrib(const Json::Value& attrib, const std::string& name, unit array[NUM_ATTRIBUTES])
{
	if (!attrib.isArray()) {
		ERR(STR_PRS << PFX << STR_ATT << PFX << name << PFX << ERR_ARR);
		return false;
	}

	if (attrib.size() != NUM_ATTRIBUTES) {
		ERR(STR_PRS << PFX << STR_ATT << PFX << name << PFX << ERR_SIZE
		            << PFX << ERR_REQ << PFX << NUM_ATTRIBUTES << SEP
		            << ERR_CURR << PFX << attrib.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_ATTRIBUTES; i++) {
		if (!attrib[i].isDouble()) {
			ERR(STR_PRS << PFX << STR_ATT << PFX << name
			            << LB << (i + 1) << RB << PFX << ERR_REAL);
			return false;
		}

		array[i] = attrib[i].asFloat();
		DBG(STR_PRS << PFX << DBG_ATT << PFX << name
		            << LB << (i + 1) << RB << PFX << FLT(array[i], 6));
	}

	return true;
}

static bool parse_json_vector(const Json::Value& vector, const std::string& name, unit array[NUM_COORDS])
{
	if (!vector.isArray()) {
		ERR(STR_PRS << PFX << STR_VEC << LB << name << RB << PFX << ERR_ARR);
		return false;
	}

	if (vector.size() != NUM_COORDS) {
		ERR(STR_PRS << PFX << STR_VEC << LB << name << RB
		            << PFX << ERR_SIZE << PFX
		            << ERR_REQ << PFX << NUM_COORDS << SEP
		            << ERR_CURR << PFX << vector.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		if (!vector[i].isDouble()) {
			ERR(STR_PRS << PFX << STR_VEC << LB << name << RB
			            << LB << (i + 1) << RB << PFX << ERR_REAL);
			return false;
		}

		array[i] = vector[i].asFloat();
		DBG(STR_PRS << PFX << DBG_VEC << LB << name << RB << PFX << STR_VEC
		            << LB << (i + 1) << RB << PFX << FLT(array[i], 6));
	}

	return true;
}

static bool parse_json_pair(const Json::Value& pair, const std::string& name, uint32_t array[NUM_POINTS])
{
	if (!pair.isArray()) {
		ERR(STR_PRS << PFX << STR_ARR << PFX << name << PFX << ERR_ARR);
		return false;
	}

	if (pair.size() != NUM_POINTS) {
		ERR(STR_PRS << PFX << STR_ARR << PFX << name << PFX << ERR_SIZE
		            << PFX << ERR_REQ << PFX << NUM_POINTS << SEP
		            << ERR_CURR << PFX << pair.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_POINTS; i++) {
		if (!pair[i].isUInt()) {
			ERR(STR_PRS << PFX << STR_ARR << PFX << name
			            << LB << (i + 1) << RB << PFX << ERR_UINT);
			return false;
		}

		array[i] = pair[i].asUInt();
		DBG(STR_PRS << PFX << DBG_PAIR << PFX << name << PFX << STR_ARR
		            << LB << (i + 1) << RB << PFX << array[i]);
	}

	return true;
}

static bool parse_json_polygon(const Json::Value& polygon, const int idx, polygon_def& pd)
{
	bool rc;

	if (!polygon.isObject()) {
		ERR(STR_PRS << PFX << STR_POLY << LB << (idx + 1) << RB
		            << PFX << ERR_OBJ);
		return false;
	}

	if (polygon.isMember(JSON_STR_NAME)) {
		if (!polygon[JSON_STR_NAME].isString()) {
			ERR(STR_PRS << PFX << STR_POLY << LB << (idx + 1) << RB
			            << PFX << JSON_STR_NAME << PFX << ERR_STR);
			return false;
		}

		pd.name = polygon[JSON_STR_NAME].asString();
		DBG(STR_PRS << PFX << DBG_POLY << LB << (idx + 1) << RB
		            << PFX << JSON_STR_NAME << PFX << pd.name);
	}

	if (polygon.isMember(JSON_STR_COLOR)) {
		if (!polygon[JSON_STR_COLOR].isUInt()) {
			ERR(STR_PRS << PFX << STR_POLY << LB << (idx + 1) << RB
			            << PFX << JSON_STR_COLOR << PFX << ERR_UINT);
			return false;
		}

		pd.color_index = polygon[JSON_STR_COLOR].asUInt();
		DBG(STR_PRS << PFX << DBG_POLY << LB << (idx + 1) << RB << PFX
		            << JSON_STR_COLOR << PFX << pd.color_index);
	}

	if (polygon.isMember(JSON_STR_FORCE)) {
		if (!polygon[JSON_STR_FORCE].isBool()) {
			ERR(STR_PRS << PFX << STR_POLY << LB << (idx + 1) << RB
			            << PFX << JSON_STR_FORCE << PFX << ERR_BOOL);
			return false;
		}

		pd.force = polygon[JSON_STR_FORCE].asBool();
		DBG(STR_PRS << PFX << DBG_POLY << LB << (idx + 1) << RB << PFX
		            << JSON_STR_FORCE << PFX << (pd.force ? "true" : "false"));
	}

	if (polygon.isMember(JSON_STR_VECS)) {
		if (!polygon[JSON_STR_VECS].isArray()) {
			ERR(STR_PRS << PFX << STR_POLY << LB << (idx + 1) << RB
			            << PFX << JSON_STR_VECS << PFX << ERR_ARR);
			return false;
		}

		const Json::Value& vectors = polygon[JSON_STR_VECS];
		DBG(STR_PRS << PFX << DBG_POLY << LB << (idx + 1) << RB << PFX
		            << JSON_STR_VECS << PFX << DBG_SIZE << PFX << vectors.size());

		unit coords[NUM_COORDS];
		int i = 0;
		for (const auto& vector : vectors) {
			rc = parse_json_vector(vector, std::to_string(i), coords);
			if (!rc) {
				ERR(STR_PRS << PFX << STR_POLY << LB << (idx + 1) << RB
				            << JSON_STR_VECS << LB << (i + 1) << RB << PFX
				            << "parse_json_vector()" << SPC << ERR_FAIL);
				return false;
			}
			i++;

			vector_3 v(coords);
			pd.vectors.push_back(v);
		}
	}

	DBG(STR_PRS << PFX << DBG_POLY << LB << (idx + 1) << RB << PFX << DBG_NUM
	            << PFX << JSON_STR_VECS << PFX << pd.vectors.size());

	return true;
}

static bool parse_json_mesh(const Json::Value& mesh, const int idx, mesh_def& md)
{
	if (!mesh.isObject()) {
		ERR(STR_PRS << PFX << JSON_STR_MESH << LB << (idx + 1) << RB
		            << PFX << ERR_OBJ);
		return false;
	}

	if (mesh.isMember(JSON_STR_NAME)) {
		if (!mesh[JSON_STR_NAME].isString()) {
			ERR(STR_PRS << PFX << JSON_STR_MESH << LB << (idx + 1) << RB
			            << PFX << JSON_STR_NAME << PFX << ERR_STR);
			return false;
		}

		md.name = mesh[JSON_STR_NAME].asString();
		DBG(STR_PRS << PFX << DBG_MESH << LB << (idx + 1) << RB << PFX
		            << JSON_STR_NAME << PFX << md.name);
	}

	if (mesh.isMember(JSON_STR_POLYS)) {
		if (!mesh[JSON_STR_POLYS].isArray()) {
			ERR(STR_PRS << PFX << JSON_STR_MESH << LB << (idx + 1) << RB
			            << PFX << JSON_STR_POLYS << PFX << ERR_ARR);
			return false;
		}

		const Json::Value& polygons = mesh[JSON_STR_POLYS];
		DBG(STR_PRS << PFX << DBG_MESH << LB << (idx + 1) << RB << PFX
		            << JSON_STR_POLYS << PFX << DBG_SIZE << PFX << polygons.size());

		int i = 0;
		for (const auto& polygon : polygons) {
			if (!polygon.isString()) {
				ERR(STR_PRS << PFX << JSON_STR_MESH << LB << (idx + 1) << RB
				            << PFX << JSON_STR_POLYS << LB << (i + 1) << RB
				            << PFX << ERR_STR);
				return false;
			}

			md.polygons.push_back(polygon.asString());
			DBG(STR_PRS << PFX << DBG_MESH << LB << (idx + 1) << RB << PFX
			            << JSON_STR_POLYS << PFX << polygon.asString());
		}
	}

	return true;
}

static bool parse_json_lib_elem_def(const Json::Value& element, const int idx, lib_elem_def& ed)
{
	if (!element.isObject()) {
		ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
		            << PFX << ERR_OBJ);
		return false;
	}

	if (element.isMember(JSON_STR_NAME)) {
		if (!element[JSON_STR_NAME].isString()) {
			ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
			            << PFX << JSON_STR_NAME << PFX << ERR_STR);
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG(STR_PRS << PFX << DBG_EDEF << LB << (idx + 1) << RB << PFX
		            << JSON_STR_NAME << PFX << ed.name);
	}

	if (element.isMember(JSON_STR_MESH)) {
		if (!element[JSON_STR_MESH].isString()) {
			ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
			            << PFX << JSON_STR_MESH << PFX << ERR_STR);
			return false;
		}

		ed.mesh = element[JSON_STR_MESH].asString();
		DBG(STR_PRS << PFX << DBG_EDEF << LB << (idx + 1) << RB << PFX
		            << JSON_STR_MESH << PFX << ed.mesh);
	}

	return true;
}

static bool parse_json_scene_elem_def(const Json::Value& element, const int idx, scene_elem_def& ed)
{
	bool rc;

	if (!element.isObject()) {
		ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
		            << PFX << ERR_OBJ);
		return false;
	}

	if (element.isMember(JSON_STR_NAME)) {
		if (!element[JSON_STR_NAME].isString()) {
			ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
			            << PFX << JSON_STR_NAME << PFX << ERR_STR);
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG(STR_PRS << PFX << DBG_EREF << LB << (idx + 1) << RB << PFX
		            << JSON_STR_NAME << PFX << ed.name);
	}

	if (element.isMember(JSON_STR_PAREN)) {
		if (element[JSON_STR_PAREN].isNull()) {
			DBG(STR_PRS << PFX << DBG_EREF << LB << (idx + 1) << RB
			            << PFX << JSON_STR_PAREN << " is null.");
			ed.parent = std::nullopt;
		}
		else if (element[JSON_STR_PAREN].isString()) {
			DBG(STR_PRS << PFX << DBG_EREF << LB << (idx + 1) << RB
			            << PFX << JSON_STR_PAREN << PFX
			            << element[JSON_STR_PAREN].asString());
			ed.parent = std::optional<std::string> {element[JSON_STR_PAREN].asString()};
		}
		else {
			ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
			            << PFX << JSON_STR_PAREN << PFX << ERR_STR);
			return false;
		}
	}

	if (element.isMember(JSON_STR_ACT)) {
		if (!element[JSON_STR_ACT].isBool()) {
			ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
			            << PFX << JSON_STR_ACT << PFX << ERR_BOOL);
			return false;
		}

		ed.active = element[JSON_STR_ACT].asBool();
		DBG(STR_PRS << PFX << DBG_EREF << LB << (idx + 1) << RB << PFX
		            << JSON_STR_ACT << PFX << (ed.active ? "true" : "false"));
	}

	if (element.isMember(JSON_STR_REF)) {
		if (!element[JSON_STR_REF].isString()) {
			ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
			            << PFX << JSON_STR_REF << PFX << ERR_STR);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_EREF << LB << (idx + 1) << RB << PFX
		            << JSON_STR_REF << PFX << element[JSON_STR_REF].asString());
		ed.ref = std::optional<std::string> {element[JSON_STR_REF].asString()};
	}

	if (element.isMember(JSON_STR_ORIG)) {
		unit array[NUM_ATTRIBUTES];

		rc = parse_json_attrib(element[JSON_STR_ORIG], JSON_STR_ORIG, array);
		if (!rc) {
			ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
			            << PFX << JSON_STR_ORIG << PFX
			            << "parse_json_attrib()" << SPC << ERR_FAIL);
			return false;
		}

		attrib att(array);
		ed.origin = att;
		att.print();
	}

	if (element.isMember(JSON_STR_MOTN)) {
		unit array[NUM_ATTRIBUTES];

		rc = parse_json_attrib(element[JSON_STR_MOTN], JSON_STR_MOTN, array);
		if (!rc) {
			ERR(STR_PRS << PFX << STR_ELEM << LB << (idx + 1) << RB
			            << PFX << JSON_STR_MOTN << PFX
			            << "parse_json_attrib()" << SPC << ERR_FAIL);
			return false;
		}

		attrib att(array);
		ed.motion = att;
		att.print();
	}

	return true;
}

static bool parse_json_library(const Json::Value& library, library_def& ld)
{
	bool rc;

	if (!library.isObject()) {
		ERR(STR_PRS << PFX << JSON_STR_LIB << PFX << ERR_OBJ);
		return false;
	}

	if (library.isMember(JSON_STR_POLYS)) {
		if (!library[JSON_STR_POLYS].isArray()) {
			ERR(STR_PRS << PFX << JSON_STR_LIB << PFX
			            << JSON_STR_POLYS << PFX << ERR_ARR);
			return false;
		}

		const Json::Value& polygons = library[JSON_STR_POLYS];
		DBG(STR_PRS << PFX << DBG_LIB << PFX << JSON_STR_POLYS << PFX
		            << DBG_SIZE << PFX << polygons.size());

		int i = 0;
		for (const auto& polygon : polygons) {
			polygon_def pd {};
			rc = parse_json_polygon(polygon, i, pd);
			if (!rc) {
				ERR(STR_PRS << PFX << JSON_STR_LIB << PFX
				            << JSON_STR_POLYS << LB << (i + 1) << RB
				            << PFX << "parse_json_polygon()" << SPC << ERR_FAIL);
				return false;
			}

			ld.polygons.push_back((std::move(pd)));
		}
	}

	if (library.isMember(JSON_STR_MESHS)) {
		if (!library[JSON_STR_MESHS].isArray()) {
			ERR(STR_PRS << PFX << JSON_STR_LIB << PFX << JSON_STR_MESHS
			            << PFX << ERR_ARR);
			return false;
		}

		const Json::Value& meshes = library[JSON_STR_MESHS];
		DBG(STR_PRS << PFX << DBG_LIB << PFX << JSON_STR_MESHS << PFX
		            << DBG_SIZE << PFX << meshes.size());

		int i = 0;
		for (const auto& mesh : meshes) {
			mesh_def md {};
			rc = parse_json_mesh(mesh, i, md);
			if (!rc) {
				ERR(STR_PRS << PFX << JSON_STR_LIB << PFX
				            << JSON_STR_MESHS << LB << (i + 1) << RB
				            << PFX << "parse_json_mesh()" << SPC << ERR_FAIL);
				return false;
			}

			ld.meshes.push_back((std::move(md)));
		}
	}

	if (library.isMember(JSON_STR_ELEMS)) {
		if (!library[JSON_STR_ELEMS].isArray()) {
			ERR(STR_PRS << PFX << JSON_STR_LIB << PFX
			            << JSON_STR_ELEMS << PFX << ERR_ARR);
			return false;
		}

		const Json::Value& elements = library[JSON_STR_ELEMS];
		DBG(STR_PRS << PFX << DBG_LIB << PFX << JSON_STR_ELEMS << PFX
		            << DBG_SIZE << PFX << elements.size());

		int i = 0;
		for (const auto& element : elements) {
			lib_elem_def ed {};
			rc = parse_json_lib_elem_def(element, i, ed);
			if (!rc) {
				ERR(STR_PRS << PFX << JSON_STR_LIB << PFX
				            << JSON_STR_ELEMS << LB << (i + 1) << RB << PFX
				            << "parse_json_lib_elem_def()" << SPC << ERR_FAIL);
				return false;
			}

			ld.lib_elem_defs.push_back(std::move(ed));
		}
	}

	DBG(STR_PRS << PFX << DBG_LIB << PFX << DBG_NUM << SPC
	            << JSON_STR_POLYS << PFX << ld.polygons.size());
	DBG(STR_PRS << PFX << DBG_LIB << PFX << DBG_NUM << SPC
	            << JSON_STR_MESHS << PFX << ld.meshes.size());
	DBG(STR_PRS << PFX << DBG_LIB << PFX << DBG_NUM << SPC
	            << JSON_STR_ELEMS << PFX << ld.lib_elem_defs.size());

	return true;
}

static bool parse_json_environment(const Json::Value& env, environ_def& ed)
{
	bool rc;

	if (!env.isObject()) {
		ERR(STR_PRS << PFX << JSON_STR_ENV << PFX << ERR_OBJ);
		return false;
	}

	if (env.isMember(JSON_STR_COLOR)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_COLOR);
		if (!env[JSON_STR_COLOR].isUInt()) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX
			            << JSON_STR_COLOR << PFX << ERR_UINT);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_COLOR << PFX
		            << env[JSON_STR_COLOR].asUInt());
		ed.color_idx = env[JSON_STR_COLOR].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_TYPE)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_LIGHT_TYPE);
		if (!env[JSON_STR_LIGHT_TYPE].isUInt()) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX
			            << JSON_STR_LIGHT_TYPE << PFX << ERR_UINT);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_LIGHT_TYPE << PFX
		            << env[JSON_STR_LIGHT_TYPE].asUInt());
		ed.light_type = env[JSON_STR_LIGHT_TYPE].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_DIR)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_LIGHT_DIR);
		unit coords[NUM_COORDS];
		rc = parse_json_vector(env[JSON_STR_LIGHT_DIR], JSON_STR_LIGHT_DIR, coords);
		if (!rc) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX << JSON_STR_LIGHT_DIR
			            << PFX << "parse_json_vector()" << SPC << ERR_FAIL);
			return false;
		}

		vector_3 v(coords);
		ed.light_direction = v;
		v.print();
	}

	if (env.isMember(JSON_STR_LIGHT_POS)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_LIGHT_POS);
		unit coords[NUM_COORDS];
		rc = parse_json_vector(env[JSON_STR_LIGHT_POS], JSON_STR_LIGHT_POS, coords);
		if (!rc) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX << JSON_STR_LIGHT_POS << PFX
			            << "parse_json_vector()" << SPC << ERR_FAIL);
			return false;
		}

		vector_3 v(coords);
		ed.light_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_CAM_POS)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_CAM_POS);
		unit coords[NUM_COORDS];
		rc = parse_json_vector(env[JSON_STR_CAM_POS], JSON_STR_CAM_POS, coords);
		if (!rc) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX << JSON_STR_CAM_POS << PFX
			            << "parse_json_vector()" << SPC << ERR_FAIL);
			return false;
		}

		vector_3 v(coords);
		ed.camera_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_FOCAL_LEN)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_FOCAL_LEN);
		if (!env[JSON_STR_FOCAL_LEN].isDouble()) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX
			            << JSON_STR_FOCAL_LEN << PFX << ERR_REAL);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_FOCAL_LEN << PFX
		            << env[JSON_STR_FOCAL_LEN].asFloat());
		ed.focal_len = env[JSON_STR_FOCAL_LEN].asFloat();
	}

	if (env.isMember(JSON_STR_NEAR_EPS)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_NEAR_EPS);
		if (!env[JSON_STR_NEAR_EPS].isDouble()) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX
			            << JSON_STR_NEAR_EPS << PFX << ERR_REAL);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_NEAR_EPS << PFX
		            << env[JSON_STR_NEAR_EPS].asFloat());
		ed.near_eps = env[JSON_STR_NEAR_EPS].asFloat();
	}

	if (env.isMember(JSON_STR_MIN_POS)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_MIN_POS);
		uint32_t points[NUM_POINTS];
		rc = parse_json_pair(env[JSON_STR_MIN_POS], JSON_STR_MIN_POS, points);
		if (!rc) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX << JSON_STR_MIN_POS << PFX
			            << "parse_json_pair()" << SPC << ERR_FAIL);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_MIN_POS << LP
		            << points[0] << SEP << points[1] << RP);
		ed.min_pos = config_ns::point_def {points[0], points[1]};
	}

	if (env.isMember(JSON_STR_MAX_POS)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_MAX_POS);
		uint32_t points[NUM_POINTS];
		rc = parse_json_pair(env[JSON_STR_MAX_POS], JSON_STR_MAX_POS, points);
		if (!rc) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX << JSON_STR_MAX_POS << PFX
			            << "parse_json_pair()" << SPC << ERR_FAIL);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_MAX_POS << LP
		            << points[0] << SEP << points[1] << RP);
		ed.max_pos = config_ns::point_def {points[0], points[1]};
	}

	if (env.isMember(JSON_STR_RES)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_RES);
		uint32_t res[NUM_POINTS];
		rc = parse_json_pair(env[JSON_STR_RES], JSON_STR_RES, res);
		if (!rc) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX << JSON_STR_RES << PFX
			            << "parse_json_pair()" << SPC << ERR_FAIL);
			return false;
		}

		// DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_RES + PFX
		//             << points[0] << 'x' << points[1]);
		resolution_def rd;
		rd.width = res[0];
		rd.height = res[1];
		ed.resolution = rd;
		DBG(STR_PRS + PFX + JSON_STR_ENV + PFX + JSON_STR_RES + PFX + std::to_string(ed.resolution.value().width) + 'x' + std::to_string(ed.resolution.value().height));
	}

	return true;
}

static bool parse_json_runtime(const Json::Value& run, runtime_def& rd)
{
	if (run.isMember(JSON_STR_LOOPS)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_LOOPS);
		if (!run[JSON_STR_LOOPS].isInt()) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX
			            << JSON_STR_LOOPS << PFX << ERR_INT);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_LOOPS << PFX
		            << run[JSON_STR_LOOPS].asInt());
		rd.loops = run[JSON_STR_LOOPS].asInt();
	}

	if (run.isMember(JSON_STR_LOOP_DELAY)) {
		DBG(STR_PRS << PFX << DBG_ENV << PFX << DBG_FOUND << SPC << JSON_STR_LOOP_DELAY);
		if (!run[JSON_STR_LOOP_DELAY].isUInt()) {
			ERR(STR_PRS << PFX << JSON_STR_ENV << PFX
			            << JSON_STR_LOOP_DELAY << PFX << ERR_UINT);
			return false;
		}

		DBG(STR_PRS << PFX << DBG_ENV << PFX << JSON_STR_LOOP_DELAY << PFX
		            << run[JSON_STR_LOOP_DELAY].asUInt());
		rd.loop_delay = run[JSON_STR_LOOP_DELAY].asUInt();
	}

	return true;
}

static bool parse_json_scene(const Json::Value& scene, const int idx, scene_def& sd)
{
	bool rc;

	if (!scene.isObject()) {
		ERR(STR_PRS << PFX << STR_SCN << LB << (idx + 1) << RB
		            << PFX << ERR_ARR);
		return false;
	}

	if (scene.isMember(JSON_STR_NAME)) {
		if (!scene[JSON_STR_NAME].isString()) {
			ERR(STR_PRS << PFX << STR_SCN << LB << (idx + 1) << RB
			            << PFX << JSON_STR_NAME << PFX << ERR_STR);
			return false;
		}

		sd.name = scene[JSON_STR_NAME].asString();
		DBG(STR_PRS << PFX << DBG_SCN << PFX << JSON_STR_NAME << PFX << sd.name);
	}

	if (scene.isMember(JSON_STR_ELEMS)) {
		if (!scene[JSON_STR_ELEMS].isArray()) {
			ERR(STR_PRS << PFX << STR_SCN << LB << (idx + 1) << RB
			            << PFX << JSON_STR_ELEMS << PFX << ERR_ARR);
			return false;
		}

		const Json::Value elements = scene[JSON_STR_ELEMS];
		DBG(STR_PRS << PFX << DBG_SCN << LB << (idx + 1) << RB << PFX
		            << JSON_STR_ELEMS << PFX << DBG_SIZE << PFX << elements.size());

		int i = 0;
		for (const auto& element : elements) {
			scene_elem_def ed {};
			rc = parse_json_scene_elem_def(element, i, ed);
			if (!rc) {
				ERR(STR_PRS << PFX << STR_SCN << LB << (idx + 1) << RB << PFX
				            << JSON_STR_ELEMS << LB << (i + 1) << RB << PFX
				            << "parse_json_scene_elem_def()" << SPC << ERR_FAIL);
				return false;
			}
			i++;

			sd.scene_elem_defs.push_back(std::move(ed));
		}
	}

	if (scene.isMember(JSON_STR_ENV)) {
		const Json::Value& env = scene[JSON_STR_ENV];
		DBG(STR_PRS << PFX << DBG_ENV << PFX << "num fields" << PFX << env.size());
		environ_def ed {};
		runtime_def rd {};

		rc = parse_json_environment(env, ed);
		if (!rc) {
			ERR(STR_PRS << PFX << STR_SCN << PFX
			            << "parse_json_environment()" << SPC << ERR_FAIL);
			return false;
		}

		rc = parse_json_runtime(env, rd);
		if (!rc) {
			ERR(STR_PRS << PFX << STR_SCN << PFX
			            << "parse_json_runtime()" << SPC << ERR_FAIL);
			return false;
		}

		sd.env = std::move(ed);
		sd.run = std::move(rd);
		DBG(STR_PRS << PFX << DBG_ENV << PFX << "parsed fields" << PFX << env.size());
	}

	return true;
}

bool parse_json(std::ifstream& ifs, config_ns::AST& ast)
{
	Json::Value root;
	Json::CharReaderBuilder builder;
	std::string errs;
	std::string err_str;
	bool rc;

	builder["collectComments"] = false;
	builder["rejectDupKeys"] = true;
	builder["allowTrailingCommas"] = false;

	rc = parseFromStream(builder, ifs, &root, &errs);
	if (!rc) {
		ERR(STR_PRS << PFX << "parseFromStream() failed with" << PFX << errs);
		return false;
	}

	if (!root.isObject()) {
		ERR(STR_PRS << PFX << "JSON root is not an object");
		return false;
	}

	// Mandatory. checked during parse
	if (!root.isMember(JSON_STR_VER)) {
		ERR(STR_PRS << PFX << JSON_STR_VER << PFX << "is missing");
		return false;
	}

	Json::Value& ver = root[JSON_STR_VER];
	uint32_t version[JSON_VER_FIELDS] = {JSON_VER_MAJ, JSON_VER_MIN, JSON_VER_REV};
	rc = check_json_version(ver, version);
	if (!rc) {
		ERR(STR_PRS << PFX << "check_json_version() failed");
		return false;
	}

	// will be caught during validate
	if (root.isMember(JSON_STR_LIB)) {
		const Json::Value& library = root[JSON_STR_LIB];
		library_def ld {};
		rc = parse_json_library(library, ld);
		if (!rc) {
			ERR(STR_PRS << PFX << "parse_json_library() failed");
			return false;
		}

		ast.library_defs = ld;
	}

	// will be caught during validate
	if (root.isMember(JSON_STR_SCNS)) {
		if (!root[JSON_STR_SCNS].isArray()) {
			ERR(STR_PRS << PFX << JSON_STR_SCNS << PFX << ERR_ARR);
			return false;
		}

		const Json::Value& scenes = root[JSON_STR_SCNS];
		DBG(STR_PRS << PFX << DBG_JSON << PFX << JSON_STR_SCNS << PFX
		            << DBG_SIZE << PFX << scenes.size());

		int i = 0;
		for (const auto& scene : scenes) {
			scene_def sd {};

			rc = parse_json_scene(scene, i, sd);
			if (!rc) {
				ERR(STR_PRS << PFX << "parse_json_scene() failed");
				return false;
			}
			i++;

			ast.scene_defs.push_back(std::move(sd));
		}
	}

	DBG(STR_PRS << PFX << DBG_JSON << PFX << DBG_NUM << SPC
	            << JSON_STR_SCNS << PFX << ast.scene_defs.size());

	return true;
}

} // namespace config_ns
