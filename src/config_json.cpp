#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

#define DEBUG_PRINTS
#include "common.h"
#include "config_json.h"
#include "utils.h"

namespace config_ns
{

// debug/error messages
// clang-format off
const std::string JSON_STR_SCN  = "scene";
const std::string JSON_STR_VEC  = "vector";
const std::string JSON_STR_PAIR = "pair";
const std::string JSON_STR_POLY = "polygon";
const std::string JSON_STR_ELEM = "element";
const std::string ERR_VER    = " unsupported version";
const std::string ERR_REQ    = "required";
const std::string ERR_CURR   = "current";
const std::string ERR_OBJ    = " is not an object";
const std::string ERR_ARRAY  = " is not an array or illegal size";
const std::string ERR_ARR    = " is not an array";
const std::string ERR_STR    = " is not a string";
const std::string ERR_UINT   = " is not an unsigned integer";
const std::string ERR_INT    = " is not an integer";
const std::string ERR_FLOAT  = " is not a floating point number";
const std::string ERR_BOOL   = " is not a boolean";
const std::string ERR_FAIL   = " failed";
#ifdef DEBUG_PRINTS
const std::string DBG_SIZE  = "size";
const std::string DBG_VER   = "check_ver";
const std::string DBG_ATT   = "parse_attrib";
const std::string DBG_VEC   = "parse_vector";
const std::string DBG_PAIR  = "parse_pair";
const std::string DBG_POLY  = "parse_polygon";
const std::string DBG_MESH  = "parse_mesh";
const std::string DBG_EDEF  = "parse_elem_def";
const std::string DBG_EREF  = "parse_elem_ref";
const std::string DBG_LIB   = "parse_lib";
const std::string DBG_ENV   = "parse_env";
const std::string DBG_SCN   = "parse_scene";
const std::string DBG_JSON  = "parse_json";
const std::string DBG_NUM   = "final num ";
const std::string DBG_FOUND = "found ";
#endif // DEBUG_PRINTS
// clang-format on

static bool check_json_version(const Json::Value& ver, const uint32_t array[JSON_VER_FIELDS])
{
	if (!ver.isArray() || ver.size() != JSON_VER_FIELDS) {
		ERR(JSON_STR_VER << ERR_ARRAY << PFX << ERR_REQ << PFX
		                 << JSON_VER_FIELDS << PFX << ERR_CURR
		                 << PFX << ver.size());
		return false;
	}

	bool match = true;
	for (uint32_t i = 0; i < JSON_VER_FIELDS; i++) {
		if (!ver[i].isUInt()) {
			ERR(JSON_STR_VER << LB << i << RB << PFX << ERR_UINT);
			return false;
		}

		if (ver[i].asUInt() != array[i]) {
			match = false;
			break;
		}
	}

	if (!match) {
		ERR(JSON_STR_VER << ERR_VER << PFX << ERR_REQ << PFX
		                 << array[0] << DOT << array[1] << DOT << array[2]
		                 << SEP << ERR_CURR << PFX << ver[0] << DOT << ver[1]
		                 << DOT << ver[2]);
		return false;
	}

	DBG(DBG_VER << PFX << "found version" << PFX
	            << array[0] << DOT << array[1] << DOT << array[2]);

	return true;
}

static bool parse_json_attrib(const Json::Value& attrib, const std::string& name, unit array[NUM_ATTRIBUTES])
{
	if (!attrib.isArray() || attrib.size() != NUM_ATTRIBUTES) {
		ERR(name << ERR_ARRAY << ERR_REQ << PFX << NUM_ATTRIBUTES
		         << SEP << ERR_CURR << PFX << attrib.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_ATTRIBUTES; i++) {
		if (!attrib[i].isDouble()) {
			ERR(name << LB << i << RB << PFX << ERR_FLOAT);
			return false;
		}

		array[i] = attrib[i].asFloat();
		DBG(DBG_ATT << PFX << name << LB << i << RB << PFX << FLT(array[i], 6));
	}

	return true;
}

static bool parse_json_vector(const Json::Value& vector, unit array[NUM_COORDS])
{
	if (!vector.isArray() || vector.size() != NUM_COORDS) {
		ERR(JSON_STR_VEC << ERR_ARRAY << ERR_REQ << PFX << NUM_COORDS << SEP
		                 << ERR_CURR << PFX << vector.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		if (!vector[i].isDouble()) {
			ERR(JSON_STR_VEC << LB << i << RB << PFX << ERR_FLOAT);
			return false;
		}

		array[i] = vector[i].asFloat();
		DBG(DBG_VEC << PFX << JSON_STR_VEC << LB << i << RB << PFX << FLT(array[i], 6));
	}

	return true;
}

static bool parse_json_pair(const Json::Value& pair, uint32_t array[NUM_POINTS])
{
	if (!pair.isArray() || pair.size() != NUM_POINTS) {
		ERR(JSON_STR_PAIR << ERR_ARRAY << ERR_REQ << PFX << NUM_POINTS << SEP
		                  << ERR_CURR << PFX << pair.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_POINTS; i++) {
		if (!pair[i].isUInt()) {
			ERR(JSON_STR_PAIR << LB << i << RB << PFX << ERR_UINT);
			return false;
		}

		array[i] = pair[i].asUInt();
		DBG(DBG_PAIR << PFX << JSON_STR_PAIR << LB << i << RB << PFX << array[i]);
	}

	return true;
}

static bool parse_json_polygon(const Json::Value& polygon, polygon_def& pd)
{
	bool rc;

	if (!polygon.isObject()) {
		ERR(JSON_STR_POLY << ERR_OBJ);
		return false;
	}

	if (polygon.isMember(JSON_STR_NAME)) {
		if (!polygon[JSON_STR_NAME].isString()) {
			ERR(JSON_STR_POLY << PFX << JSON_STR_NAME << ERR_STR);
			return false;
		}

		pd.name = polygon[JSON_STR_NAME].asString();
		DBG(DBG_POLY << PFX << JSON_STR_NAME << PFX << pd.name);
	}

	if (polygon.isMember(JSON_STR_COLOR)) {
		if (!polygon[JSON_STR_COLOR].isUInt()) {
			ERR(JSON_STR_POLY << PFX << JSON_STR_COLOR << ERR_UINT);
			return false;
		}

		pd.color_index = polygon[JSON_STR_COLOR].asUInt();
		DBG(DBG_POLY << PFX << JSON_STR_COLOR << PFX << pd.color_index);
	}

	if (polygon.isMember(JSON_STR_FORCE)) {
		if (!polygon[JSON_STR_FORCE].isBool()) {
			ERR(JSON_STR_POLY << PFX << JSON_STR_FORCE << ERR_BOOL);
			return false;
		}

		pd.force = polygon[JSON_STR_FORCE].asBool();
		DBG(DBG_POLY << PFX << JSON_STR_FORCE << PFX << (pd.force ? "true" : "false"));
	}

	if (polygon.isMember(JSON_STR_VECS)) {
		if (!polygon[JSON_STR_VECS].isArray()) {
			ERR(JSON_STR_POLY << PFX << JSON_STR_VECS << ERR_ARR);
			return false;
		}

		const Json::Value& vectors = polygon[JSON_STR_VECS];
		DBG(DBG_POLY << PFX << JSON_STR_VECS << PFX << DBG_SIZE << PFX << vectors.size());

		unit coords[NUM_COORDS];
		for (const auto& vector : vectors) {
			rc = parse_json_vector(vector, coords);
			if (!rc) {
				ERR(JSON_STR_POLY << PFX << JSON_STR_VECS << PFX
				                  << "parse_json_vector()" << ERR_FAIL);
				return false;
			}

			vector_3 v(coords);
			pd.vectors.push_back(v);
		}
	}

	DBG(DBG_POLY << PFX << DBG_NUM << JSON_STR_VECS << PFX << pd.vectors.size());

	return true;
}

static bool parse_json_mesh(const Json::Value& mesh, mesh_def& md)
{
	if (!mesh.isObject()) {
		ERR(JSON_STR_MESH << ERR_OBJ);
		return false;
	}

	if (mesh.isMember(JSON_STR_NAME)) {
		if (!mesh[JSON_STR_NAME].isString()) {
			ERR(JSON_STR_MESH << PFX << JSON_STR_NAME << ERR_STR);
			return false;
		}

		md.name = mesh[JSON_STR_NAME].asString();
		DBG(DBG_MESH << PFX << JSON_STR_NAME << PFX << md.name);
	}

	if (mesh.isMember(JSON_STR_POLYS)) {
		if (!mesh[JSON_STR_POLYS].isArray()) {
			ERR(JSON_STR_MESH << PFX << JSON_STR_POLYS << ERR_ARR);
			return false;
		}

		const Json::Value& polygons = mesh[JSON_STR_POLYS];
		DBG(DBG_MESH << PFX << JSON_STR_POLYS << PFX << DBG_SIZE << PFX << polygons.size());

		for (const auto& polygon : polygons) {
			if (!polygon.isString()) {
				ERR(JSON_STR_MESH << PFX << JSON_STR_POLYS << PFX << ERR_STR);
				return false;
			}

			md.polygons.push_back(polygon.asString());
			DBG(DBG_MESH << PFX << JSON_STR_POLYS << PFX << polygon.asString());
		}
	}

	return true;
}

static bool parse_json_lib_elem_def(const Json::Value& element, lib_elem_def& ed)
{
	if (!element.isObject()) {
		ERR(JSON_STR_ELEM << ERR_OBJ);
		return false;
	}

	if (element.isMember(JSON_STR_NAME)) {
		if (!element[JSON_STR_NAME].isString()) {
			ERR(JSON_STR_ELEM << PFX << JSON_STR_NAME << ERR_STR);
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG(DBG_EDEF << PFX << JSON_STR_NAME << PFX << ed.name);
	}

	if (element.isMember(JSON_STR_MESH)) {
		if (!element[JSON_STR_MESH].isString()) {
			ERR(JSON_STR_ELEM << PFX << JSON_STR_MESH << ERR_STR);
			return false;
		}

		ed.mesh = element[JSON_STR_MESH].asString();
		DBG(DBG_EDEF << PFX << JSON_STR_MESH << PFX << ed.mesh);
	}

	return true;
}

static bool parse_json_scene_elem_def(const Json::Value& element, scene_elem_def& ed)
{
	bool rc;

	if (!element.isObject()) {
		ERR(JSON_STR_ELEM << ERR_OBJ);
		return false;
	}

	if (element.isMember(JSON_STR_NAME)) {
		if (!element[JSON_STR_NAME].isString()) {
			ERR(JSON_STR_ELEM << PFX << JSON_STR_NAME << ERR_STR);
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG(DBG_EREF << PFX << JSON_STR_NAME << PFX << ed.name);
	}

	if (element.isMember(JSON_STR_PAREN)) {
		if (element[JSON_STR_PAREN].isNull()) {
			DBG(DBG_EREF << PFX << JSON_STR_PAREN << " is null.");
			ed.parent = std::nullopt;
		}
		else if (element[JSON_STR_PAREN].isString()) {
			DBG(DBG_EREF << PFX << JSON_STR_PAREN << PFX << element[JSON_STR_PAREN].asString());
			ed.parent = std::optional<std::string> {element[JSON_STR_PAREN].asString()};
		}
		else {
			ERR(JSON_STR_ELEM << PFX << JSON_STR_PAREN << ERR_STR);
			return false;
		}
	}

	if (element.isMember(JSON_STR_ACT)) {
		if (!element[JSON_STR_ACT].isBool()) {
			ERR(JSON_STR_ELEM << PFX << JSON_STR_ACT << ERR_BOOL);
			return false;
		}

		ed.active = element[JSON_STR_ACT].asBool();
		DBG(DBG_EREF << PFX << JSON_STR_ACT << PFX << (ed.active ? "true" : "false"));
	}

	if (element.isMember(JSON_STR_REF)) {
		if (!element[JSON_STR_REF].isString()) {
			ERR(JSON_STR_ELEM << PFX << JSON_STR_REF << ERR_STR);
			return false;
		}

		DBG(DBG_EREF << PFX << JSON_STR_REF << PFX << element[JSON_STR_REF].asString());
		ed.ref = std::optional<std::string> {element[JSON_STR_REF].asString()};
	}

	if (element.isMember(JSON_STR_ORIG)) {
		unit array[NUM_ATTRIBUTES];

		rc = parse_json_attrib(element[JSON_STR_ORIG], JSON_STR_ORIG, array);
		if (!rc) {
			ERR(JSON_STR_ELEM << PFX << JSON_STR_ORIG << PFX
			                  << "parse_json_attrib()" << ERR_FAIL);
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
			ERR(JSON_STR_ELEM << PFX << JSON_STR_MOTN << PFX
			                  << "parse_json_attrib()" << ERR_FAIL);
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
		ERR(JSON_STR_LIB << ERR_ARR);
		return false;
	}

	if (library.isMember(JSON_STR_POLYS)) {
		if (!library[JSON_STR_POLYS].isArray()) {
			ERR(JSON_STR_LIB << PFX << JSON_STR_POLYS << ERR_ARR);
			return false;
		}

		const Json::Value& polygons = library[JSON_STR_POLYS];
		DBG(DBG_LIB << PFX << JSON_STR_POLYS << PFX << DBG_SIZE << PFX << polygons.size());

		for (const auto& polygon : polygons) {
			polygon_def pd {};
			rc = parse_json_polygon(polygon, pd);
			if (!rc) {
				ERR(JSON_STR_LIB << PFX << JSON_STR_POLYS << PFX
				                 << "parse_json_polygon()" << ERR_FAIL);
				return false;
			}

			ld.polygons.push_back((std::move(pd)));
		}
	}

	if (library.isMember(JSON_STR_MESHS)) {
		if (!library[JSON_STR_MESHS].isArray()) {
			ERR(JSON_STR_LIB << PFX << JSON_STR_MESHS << ERR_ARR);
			return false;
		}

		const Json::Value& meshes = library[JSON_STR_MESHS];
		DBG(DBG_LIB << PFX << JSON_STR_MESHS << PFX << DBG_SIZE << PFX << meshes.size());

		for (const auto& mesh : meshes) {
			mesh_def md {};
			rc = parse_json_mesh(mesh, md);
			if (!rc) {
				ERR(JSON_STR_LIB << PFX << JSON_STR_MESHS << PFX
				                 << "parse_json_mesh()" << ERR_FAIL);
				return false;
			}

			ld.meshes.push_back((std::move(md)));
		}
	}

	if (library.isMember(JSON_STR_ELEMS)) {
		if (!library[JSON_STR_ELEMS].isArray()) {
			ERR(JSON_STR_LIB << PFX << JSON_STR_ELEMS << ERR_ARR);
			return false;
		}

		const Json::Value& elements = library[JSON_STR_ELEMS];
		DBG(DBG_LIB << PFX << JSON_STR_ELEMS << PFX << DBG_SIZE << PFX << elements.size());

		for (const auto& element : elements) {
			lib_elem_def ed {};
			rc = parse_json_lib_elem_def(element, ed);
			if (!rc) {
				ERR(JSON_STR_LIB << PFX << JSON_STR_ELEMS << PFX
				                 << "parse_json_lib_elem_def()"
				                 << ERR_FAIL);
				return false;
			}

			ld.lib_elem_defs.push_back(std::move(ed));
		}
	}

	DBG(DBG_LIB << PFX << DBG_NUM << JSON_STR_POLYS << PFX << ld.polygons.size());
	DBG(DBG_LIB << PFX << DBG_NUM << JSON_STR_MESHS << PFX << ld.meshes.size());
	DBG(DBG_LIB << PFX << DBG_NUM << JSON_STR_ELEMS << PFX << ld.lib_elem_defs.size());

	return true;
}

static bool parse_json_environment(const Json::Value& env, environ_def& ed, runtime_def& rd)
{
	bool rc;

	if (!env.isObject()) {
		ERR(JSON_STR_ENV << ERR_OBJ);
		return false;
	}

	if (env.isMember(JSON_STR_COLOR)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_COLOR);
		if (!env[JSON_STR_COLOR].isUInt()) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_COLOR << ERR_UINT);
			return false;
		}

		DBG(DBG_ENV << PFX << JSON_STR_COLOR << PFX << env[JSON_STR_COLOR].asUInt());
		ed.color_idx = env[JSON_STR_COLOR].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_TYPE)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_LIGHT_TYPE);
		if (!env[JSON_STR_LIGHT_TYPE].isUInt()) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_LIGHT_TYPE << ERR_UINT);
			return false;
		}

		DBG(DBG_ENV << PFX << JSON_STR_LIGHT_TYPE << PFX << env[JSON_STR_LIGHT_TYPE].asUInt());
		ed.light_type = env[JSON_STR_LIGHT_TYPE].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_DIR)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_LIGHT_DIR);
		unit coords[NUM_COORDS];
		rc = parse_json_vector(env[JSON_STR_LIGHT_DIR], coords);
		if (!rc) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_LIGHT_DIR
			                 << "parse_json_vector()" << ERR_FAIL);
			return false;
		}

		vector_3 v(coords);
		ed.light_direction = v;
		v.print();
	}

	if (env.isMember(JSON_STR_LIGHT_POS)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_LIGHT_POS);
		unit coords[NUM_COORDS];
		rc = parse_json_vector(env[JSON_STR_LIGHT_POS], coords);
		if (!rc) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_LIGHT_POS
			                 << "parse_json_vector()" << ERR_FAIL);
			return false;
		}

		vector_3 v(coords);
		ed.light_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_CAM_POS)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_CAM_POS);
		unit coords[NUM_COORDS];
		rc = parse_json_vector(env[JSON_STR_CAM_POS], coords);
		if (!rc) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_CAM_POS
			                 << "parse_json_vector()" << ERR_FAIL);
			return false;
		}

		vector_3 v(coords);
		ed.camera_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_FOCAL_LEN)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_FOCAL_LEN);
		if (!env[JSON_STR_FOCAL_LEN].isDouble()) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_FOCAL_LEN << ERR_FLOAT);
			return false;
		}

		DBG(DBG_ENV << PFX << JSON_STR_FOCAL_LEN << PFX << env[JSON_STR_FOCAL_LEN].asFloat());
		ed.focal_len = env[JSON_STR_FOCAL_LEN].asFloat();
	}

	if (env.isMember(JSON_STR_NEAR_EPS)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_NEAR_EPS);
		if (!env[JSON_STR_NEAR_EPS].isDouble()) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_NEAR_EPS << ERR_FLOAT);
			return false;
		}

		DBG(DBG_ENV << PFX << JSON_STR_NEAR_EPS << PFX << env[JSON_STR_NEAR_EPS].asFloat());
		ed.near_eps = env[JSON_STR_NEAR_EPS].asFloat();
	}

	if (env.isMember(JSON_STR_LOOPS)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_LOOPS);
		if (!env[JSON_STR_LOOPS].isInt()) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_LOOPS << ERR_INT);
			return false;
		}

		DBG(DBG_ENV << PFX << JSON_STR_LOOPS << PFX << env[JSON_STR_LOOPS].asInt());
		rd.loops = env[JSON_STR_LOOPS].asInt();
	}

	if (env.isMember(JSON_STR_LOOP_DELAY)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_LOOP_DELAY);
		if (!env[JSON_STR_LOOP_DELAY].isUInt()) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_LOOP_DELAY << ERR_UINT);
			return false;
		}

		DBG(DBG_ENV << PFX << JSON_STR_LOOP_DELAY << PFX << env[JSON_STR_LOOP_DELAY].asUInt());
		rd.loop_delay = env[JSON_STR_LOOP_DELAY].asUInt();
	}

	if (env.isMember(JSON_STR_MIN_POS)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_MIN_POS);
		uint32_t points[NUM_POINTS];
		rc = parse_json_pair(env[JSON_STR_MIN_POS], points);
		if (!rc) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_MIN_POS
			                 << "parse_json_pair()" << ERR_FAIL);
			return false;
		}

		DBG(DBG_ENV << PFX << JSON_STR_MIN_POS << LP << points[0] << SEP << points[1] << RP);
		ed.min_pos = config_ns::point_def {points[0], points[1]};
	}

	if (env.isMember(JSON_STR_MAX_POS)) {
		DBG(DBG_ENV << PFX << DBG_FOUND << PFX << JSON_STR_MAX_POS);
		uint32_t points[NUM_POINTS];
		rc = parse_json_pair(env[JSON_STR_MAX_POS], points);
		if (!rc) {
			ERR(JSON_STR_ENV << PFX << JSON_STR_MAX_POS
			                 << "parse_json_pair()" << ERR_FAIL);
			return false;
		}

		DBG(DBG_ENV << PFX << JSON_STR_MAX_POS << LP << points[0] << SEP << points[1] << RP);
		ed.max_pos = config_ns::point_def {points[0], points[1]};
	}

	return true;
}

static bool parse_json_scene(const Json::Value& scene, scene_def& sd)
{
	bool rc;

	if (!scene.isObject()) {
		ERR(JSON_STR_SCN << ERR_ARR);
		return false;
	}

	if (scene.isMember(JSON_STR_NAME)) {
		if (!scene[JSON_STR_NAME].isString()) {
			ERR(JSON_STR_SCN << PFX << JSON_STR_NAME << ERR_STR);
			return false;
		}

		sd.name = scene[JSON_STR_NAME].asString();
		DBG(DBG_SCN << PFX << JSON_STR_NAME << PFX << sd.name);
	}

	if (scene.isMember(JSON_STR_ELEMS)) {
		if (!scene[JSON_STR_ELEMS].isArray()) {
			ERR(JSON_STR_SCN << PFX << JSON_STR_ELEMS << ERR_ARR);
			return false;
		}

		const Json::Value elements = scene[JSON_STR_ELEMS];
		DBG(DBG_SCN << PFX << JSON_STR_ELEMS << PFX << DBG_SIZE << PFX << elements.size());

		for (const auto& element : elements) {
			scene_elem_def ed {};
			rc = parse_json_scene_elem_def(element, ed);
			if (!rc) {
				ERR(JSON_STR_SCN << PFX << JSON_STR_ELEMS << PFX
				                 << "parse_json_scene_elem_def()"
				                 << ERR_FAIL);
				return false;
			}

			sd.scene_elem_defs.push_back(std::move(ed));
		}
	}

	if (scene.isMember(JSON_STR_ENV)) {
		const Json::Value& env = scene[JSON_STR_ENV];
		DBG(DBG_ENV << PFX << DBG_NUM << "fields" << PFX << env.size());
		environ_def ed {};
		runtime_def rd {};

		rc = parse_json_environment(env, ed, rd);
		if (!rc) {
			ERR(JSON_STR_SCN << PFX << "parse_json_environment()"
			                 << ERR_FAIL);
			return false;
		}

		sd.env = std::move(ed);
		sd.run = std::move(rd);
	}

	DBG(JSON_STR_SCN << PFX << DBG_NUM << JSON_STR_ELEMS << PFX << sd.scene_elem_defs.size());
	return true;
}

AST parse_json(const std::string& filename)
{
	AST ast;
	bool rc;
	std::ifstream ifs;
	Json::Value root;
	Json::CharReaderBuilder builder;
	JSONCPP_STRING errs;
	std::string err_str;

	DBG(DBG_JSON << PFX << "file name" << PFX << filename);
	ifs.open(filename.c_str(), std::ios::in);
	if (!ifs) {
		sys_error("file not found: ", filename.c_str());
	}

	ifs.unsetf(std::ios::skipws);
	builder["collectComments"] = false;

	rc = parseFromStream(builder, ifs, &root, &errs);
	ifs.close();
	if (!rc) {
		sys_error("parseFromStream() failed with: ", errs.c_str());
	}

	// Mandatory. checked during parse
	if (!root.isMember(JSON_STR_VER)) {
		std::string s = JSON_STR_VER + " is missing in: " + filename;
		sys_error(s.c_str());
	}

	Json::Value& ver = root[JSON_STR_VER];
	uint32_t version[JSON_VER_FIELDS] = {JSON_VER_MAJ, JSON_VER_MIN, JSON_VER_REV};
	rc = check_json_version(ver, version);
	if (!rc) {
		sys_error("check_json_version() failed in ", filename.c_str());
	}

	// will be caught during validate
	if (root.isMember(JSON_STR_LIB)) {
		const Json::Value& library = root[JSON_STR_LIB];
		library_def ld {};
		rc = parse_json_library(library, ld);
		if (!rc) {
			sys_error("parse_json_library() failed in ", filename.c_str());
		}

		ast.library_defs = ld;
	}

	// will be caught during validate
	if (root.isMember(JSON_STR_SCNS)) {
		if (!root[JSON_STR_SCNS].isArray()) {
			std::string s = JSON_STR_SCNS + ERR_ARR + PFX + filename;
			sys_error(s.c_str());
		}

		const Json::Value& scenes = root[JSON_STR_SCNS];
		DBG(DBG_JSON << PFX << JSON_STR_SCNS << PFX << DBG_SIZE << PFX << scenes.size());

		for (const auto& scene : scenes) {
			scene_def sd {};

			rc = parse_json_scene(scene, sd);
			if (!rc) {
				sys_error("parse_json_scenes() failed in: ", filename.c_str());
			}

			ast.scene_defs.push_back(std::move(sd));
		}
	}

	DBG(DBG_JSON << PFX << DBG_NUM << JSON_STR_SCNS << PFX << ast.scene_defs.size());

	return ast;
}

} // namespace config_ns
