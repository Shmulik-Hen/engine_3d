#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

#define DEBUG_PRINTS
#include "utils.h"
#include "parse_json.h"

namespace config_ns
{

static bool check_json_version(const Json::Value& ver, const std::string& path, const uint32_t array[JSON_VER_FIELDS])
{
	if (ver.size() != JSON_VER_FIELDS) {
		// clang-format off
		ERR(path + ERR_SIZE + PFX + ERR_REQ + PFX +
		    std::to_string(JSON_VER_FIELDS) + SEP +
		    ERR_CURR + PFX + std::to_string(ver.size()));
		// clang-format on
		return false;
	}

	bool match = true;
	for (uint32_t i = 0; i < JSON_VER_FIELDS; i++) {
		if (!ver[i].isUInt()) {
			// clang-format off
			ERR(path + JSON_STR_VER +
			    LB + std::to_string(i + 1) + RB +
			    PFX + ERR_UINT);
			// clang-format on
			return false;
		}

		if (ver[i].asUInt() != array[i]) {
			match = false;
			break;
		}
	}

	if (!match) {
		// clang-format off
		ERR(path + ERR_VER + PFX +
		    ERR_REQ + PFX +
		    std::to_string(array[0]) + DOT +
		    std::to_string(array[1]) + DOT +
		    std::to_string(array[2]) + SEP +
		    ERR_CURR + PFX +
		    ver[0].asString() + DOT +
		    ver[1].asString() + DOT +
		    ver[2].asString());
		// clang-format on
		return false;
	}

	// clang-format off
	DBG(path + "found version" + PFX +
	    std::to_string(array[0]) + DOT +
	    std::to_string(array[1]) + DOT +
	    std::to_string(array[2]));
	// clang-format on

	return true;
}

static bool parse_json_attrib(const Json::Value& att, const std::string& path, unit array[NUM_ATTRIBUTES])
{
	if (att.size() != NUM_ATTRIBUTES) {
		// clang-format off
		ERR(path + ERR_SIZE + PFX +
		    ERR_REQ + PFX + std::to_string(NUM_ATTRIBUTES) + SEP +
		    ERR_CURR + PFX + std::to_string(att.size()));
		// clang-format on
		return false;
	}

	for (uint32_t i = 0; i < NUM_ATTRIBUTES; i++) {
		std::string str = path + get_att_name(i);
		if (!att[i].isDouble()) {
			// clang-format off
			ERR(str + ERR_REAL);
			// clang-format on
			return false;
		}

		DBG(str + set_precision(att[i].asFloat()));
		array[i] = att[i].asFloat();
	}

	return true;
}

static bool parse_json_vector(const Json::Value& vector, const std::string& path, unit array[NUM_COORDS])
{
	if (vector.size() != NUM_COORDS) {
		// clang-format off
		ERR(path + ERR_SIZE + PFX +
		    ERR_REQ + PFX + std::to_string(NUM_COORDS) + SEP +
		    ERR_CURR + PFX + std::to_string(vector.size()));
		// clang-format on
		return false;
	}

	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		// clang-format off
		std::string str = path + STR_CORD + LB + get_coord_name(i) + RB;
		// clang-format on
		if (!vector[i].isDouble()) {
			ERR(str + PFX + ERR_REAL);
			return false;
		}

		array[i] = vector[i].asFloat();
		DBG(str + PFX + set_precision(array[i]));
	}

	return true;
}

static bool parse_json_pair(const Json::Value& pair, const std::string& path, uint32_t array[NUM_POINTS])
{
	if (pair.size() != NUM_POINTS) {
		// clang-format off
		ERR(path + ERR_SIZE + PFX +
		    ERR_REQ + PFX + std::to_string(NUM_POINTS) + SEP +
		    ERR_CURR + PFX + std::to_string(pair.size()));
		// clang-format on
		return false;
	}

	for (uint32_t i = 0; i < NUM_POINTS; i++) {
		std::string str = path + STR_PAIR + LB + std::to_string(i + 1) + RB;
		if (!pair[i].isUInt()) {
			ERR(str + PFX + ERR_UINT);
			return false;
		}

		array[i] = pair[i].asUInt();
		DBG(str + PFX + std::to_string(array[i]));
	}

	return true;
}

static bool parse_json_polygon(const Json::Value& polygon, const std::string path, polygon_def& pd)
{
	bool rc;

	if (polygon.isMember(JSON_STR_NAME)) {
		if (!polygon[JSON_STR_NAME].isString()) {
			ERR(path + JSON_STR_NAME + PFX + ERR_STR);
			return false;
		}

		pd.name = polygon[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + QT + pd.name + QT);
	}

	if (polygon.isMember(JSON_STR_COLOR)) {
		if (!polygon[JSON_STR_COLOR].isUInt()) {
			ERR(path + JSON_STR_COLOR + PFX + ERR_UINT);
			return false;
		}

		pd.color_index = polygon[JSON_STR_COLOR].asUInt();
		DBG(path + JSON_STR_COLOR + PFX + std::to_string(pd.color_index));
	}

	if (polygon.isMember(JSON_STR_FORCE)) {
		if (!polygon[JSON_STR_FORCE].isBool()) {
			ERR(path + JSON_STR_FORCE + PFX + ERR_BOOL);
			return false;
		}

		pd.force = polygon[JSON_STR_FORCE].asBool();
		DBG(path + JSON_STR_FORCE + PFX << BOOL(pd.force));
	}

	if (polygon.isMember(JSON_STR_VECS)) {
		if (!polygon[JSON_STR_VECS].isArray()) {
			ERR(path + JSON_STR_VECS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& vectors = polygon[JSON_STR_VECS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_VECS +
		    PFX + std::to_string(vectors.size()));
		// clang-format on

		int i = 1;
		for (const auto& vector : vectors) {
			std::string str = path + STR_VEC + LB + std::to_string(i) + RB;
			unit coords[NUM_COORDS];

			if (!vector.isArray()) {
				ERR(str + PFX + ERR_ARR);
				return false;
			}

			rc = parse_json_vector(vector, str + PFX, coords);
			if (!rc) {
				return false;
			}
			i++;

			vector_3 v(coords);
			pd.vectors.push_back(v);
		}
	}

	return true;
}

static bool parse_json_mesh(const Json::Value& mesh, const std::string path, mesh_def& md)
{
	if (mesh.isMember(JSON_STR_NAME)) {
		if (!mesh[JSON_STR_NAME].isString()) {
			ERR(path + JSON_STR_NAME + PFX + ERR_STR);
			return false;
		}

		md.name = mesh[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + QT + md.name + QT);
	}

	if (mesh.isMember(JSON_STR_POLYS)) {
		if (!mesh[JSON_STR_POLYS].isArray()) {
			ERR(path + JSON_STR_POLYS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& polygons = mesh[JSON_STR_POLYS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_POLYS +
		    PFX + std::to_string(polygons.size()));
		// clang-format on

		int i = 1;
		for (const auto& polygon : polygons) {
			if (!polygon.isString()) {
				// clang-format off
				ERR(path + STR_POLY +
				    LB + std::to_string(i) + RB +
				    PFX + ERR_STR);
				// clang-format on
				return false;
			}

			md.polygons.push_back(polygon.asString());
			// clang-format off
			DBG(path + STR_POLY +
			    LB + std::to_string(i) + RB +
			    PFX + QT + polygon.asString() + QT);
			// clang-format on
		}
	}

	return true;
}

static bool parse_json_lib_elem_def(const Json::Value& element, const std::string path, lib_elem_def& ed)
{
	if (element.isMember(JSON_STR_NAME)) {
		if (!element[JSON_STR_NAME].isString()) {
			ERR(path + JSON_STR_NAME + PFX + ERR_STR);
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + QT + ed.name + QT);
	}

	if (element.isMember(JSON_STR_MESH)) {
		if (!element[JSON_STR_MESH].isString()) {
			ERR(path + JSON_STR_MESH + PFX + ERR_STR);
			return false;
		}

		ed.mesh = element[JSON_STR_MESH].asString();
		DBG(path + JSON_STR_MESH + PFX + QT + ed.mesh + QT);
	}

	return true;
}

static bool parse_json_scene_elem_def(const Json::Value& element, const std::string path, scene_elem_def& ed)
{
	std::string str;
	bool rc;

	if (element.isMember(JSON_STR_NAME)) {
		str = path + JSON_STR_NAME;
		if (!element[JSON_STR_NAME].isString()) {
			ERR(str + PFX + ERR_STR);
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + QT + ed.name + QT);
	}

	if (element.isMember(JSON_STR_PAREN)) {
		str = path + JSON_STR_PAREN;
		if (element[JSON_STR_PAREN].isNull()) {
			DBG(str + ERR_NULL);
			ed.parent = std::nullopt;
		}
		else if (element[JSON_STR_PAREN].isString()) {
			DBG(str + QT + element[JSON_STR_PAREN].asString() + QT);
			ed.parent = std::optional<std::string> {element[JSON_STR_PAREN].asString()};
		}
		else {
			ERR(str + PFX + ERR_STR);
			return false;
		}
	}

	if (element.isMember(JSON_STR_ACT)) {
		str = path + JSON_STR_ACT;
		if (!element[JSON_STR_ACT].isBool()) {
			ERR(str + PFX + ERR_BOOL);
			return false;
		}

		ed.active = element[JSON_STR_ACT].asBool();
		DBG(str + PFX + BOOL(ed.active));
	}

	if (element.isMember(JSON_STR_REF)) {
		str = path + JSON_STR_REF;
		if (!element[JSON_STR_REF].isString()) {
			ERR(str + PFX + ERR_STR);
			return false;
		}

		DBG(str + PFX + QT + element[JSON_STR_REF].asString() + QT);
		ed.ref = std::optional<std::string> {element[JSON_STR_REF].asString()};
	}

	if (element.isMember(JSON_STR_ORIG)) {
		str = path + JSON_STR_ORIG;
		unit array[NUM_ATTRIBUTES];

		if (!element[JSON_STR_ORIG].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_attrib(element[JSON_STR_ORIG], str + PFX, array);
		if (!rc) {
			return false;
		}

		attrib att(array);
		ed.origin = att;
		att.print();
	}

	if (element.isMember(JSON_STR_MOTN)) {
		str = path + JSON_STR_MOTN;
		unit array[NUM_ATTRIBUTES];

		if (!element[JSON_STR_MOTN].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_attrib(element[JSON_STR_MOTN], str + PFX, array);
		if (!rc) {
			return false;
		}

		attrib att(array);
		ed.motion = att;
		att.print();
	}

	return true;
}

static bool parse_json_environment(const Json::Value& env, const std::string path, environ_def& ed)
{
	std::string str;
	bool rc;

	if (env.isMember(JSON_STR_COLOR)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_COLOR);

		str = path + JSON_STR_COLOR;
		if (!env[JSON_STR_COLOR].isUInt()) {
			ERR(str + PFX + ERR_UINT);
			return false;
		}

		DBG(str + PFX + std::to_string(env[JSON_STR_COLOR].asUInt()));
		ed.color_idx = env[JSON_STR_COLOR].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_TYPE)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LIGHT_TYPE);

		str = path + JSON_STR_LIGHT_TYPE;
		if (!env[JSON_STR_LIGHT_TYPE].isUInt()) {
			ERR(str + PFX + ERR_UINT);
			return false;
		}

		DBG(str + PFX + std::to_string(env[JSON_STR_LIGHT_TYPE].asUInt()));
		ed.light_type = env[JSON_STR_LIGHT_TYPE].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_DIR)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LIGHT_DIR);

		str = path + JSON_STR_LIGHT_DIR;
		unit coords[NUM_COORDS];

		if (!env[JSON_STR_LIGHT_DIR].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_vector(env[JSON_STR_LIGHT_DIR], str + PFX, coords);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ed.light_direction = v;
		v.print();
	}

	if (env.isMember(JSON_STR_LIGHT_POS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LIGHT_POS);

		str = path + JSON_STR_LIGHT_DIR;
		unit coords[NUM_COORDS];

		if (!env[JSON_STR_LIGHT_POS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_vector(env[JSON_STR_LIGHT_POS], str + PFX, coords);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ed.light_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_CAM_POS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_CAM_POS);

		str = path + JSON_STR_CAM_POS;
		unit coords[NUM_COORDS];

		if (!env[JSON_STR_CAM_POS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_vector(env[JSON_STR_CAM_POS], str + PFX, coords);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ed.camera_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_FOCAL_LEN)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_FOCAL_LEN);

		str = path + JSON_STR_FOCAL_LEN;
		if (!env[JSON_STR_FOCAL_LEN].isDouble()) {
			ERR(str + PFX + ERR_REAL);
			return false;
		}

		DBG(str + PFX + std::to_string(env[JSON_STR_FOCAL_LEN].asFloat()));
		ed.focal_len = env[JSON_STR_FOCAL_LEN].asFloat();
	}

	if (env.isMember(JSON_STR_NEAR_EPS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_NEAR_EPS);

		str = path + JSON_STR_NEAR_EPS;
		if (!env[JSON_STR_NEAR_EPS].isDouble()) {
			ERR(str + PFX + ERR_REAL);
			return false;
		}

		DBG(str + PFX + std::to_string(env[JSON_STR_NEAR_EPS].asFloat()));
		ed.near_eps = env[JSON_STR_NEAR_EPS].asFloat();
	}

	if (env.isMember(JSON_STR_MIN_POS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_MIN_POS);

		str = path + JSON_STR_MIN_POS;
		uint32_t points[NUM_POINTS];

		if (!env[JSON_STR_MIN_POS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_pair(env[JSON_STR_MIN_POS], str + PFX, points);
		if (!rc) {
			return false;
		}

		DBG(str + LP + std::to_string(points[0]) + SEP + std::to_string(points[1]) + RP);
		ed.min_pos = config_ns::point_def {points[0], points[1]};
	}

	if (env.isMember(JSON_STR_MAX_POS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_MAX_POS);

		str = path + JSON_STR_MAX_POS;
		uint32_t points[NUM_POINTS];

		if (!env[JSON_STR_MAX_POS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_pair(env[JSON_STR_MAX_POS], str + PFX, points);
		if (!rc) {
			return false;
		}

		DBG(str + LP + std::to_string(points[0]) + SEP + std::to_string(points[1]) + RP);
		ed.max_pos = config_ns::point_def {points[0], points[1]};
	}

	if (env.isMember(JSON_STR_RES)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_RES);

		str = path + JSON_STR_RES;
		uint32_t res[NUM_POINTS];

		if (!env[JSON_STR_RES].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		rc = parse_json_pair(env[JSON_STR_RES], str + PFX, res);
		if (!rc) {
			return false;
		}

		DBG(str + PFX + std::to_string(res[0]) + 'x' + std::to_string(res[1]));
		ed.resolution = config_ns::resolution_def {res[0], res[1]};
	}

	return true;
}

static bool parse_json_runtime(const Json::Value& run, const std::string path, runtime_def& rd)
{
	std::string str;

	if (run.isMember(JSON_STR_LOOPS)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LOOPS);
		str = path + JSON_STR_LOOPS;
		if (!run[JSON_STR_LOOPS].isInt()) {
			ERR(str + PFX + ERR_INT);
			return false;
		}

		DBG(str + PFX + std::to_string(run[JSON_STR_LOOPS].asInt()));
		rd.loops = run[JSON_STR_LOOPS].asInt();
	}

	if (run.isMember(JSON_STR_LOOP_DELAY)) {
		DBG(path + STR_FOUND + SPC + JSON_STR_LOOP_DELAY);
		str = path + JSON_STR_LOOP_DELAY;
		if (!run[JSON_STR_LOOP_DELAY].isUInt()) {
			ERR(str + PFX + ERR_UINT);
			return false;
		}

		DBG(str + PFX + std::to_string(run[JSON_STR_LOOP_DELAY].asUInt()));
		rd.loop_delay = run[JSON_STR_LOOP_DELAY].asUInt();
	}

	return true;
}

static bool parse_json_library(const Json::Value& library, const std::string path, library_def& ld)
{
	bool rc;

	if (library.isMember(JSON_STR_POLYS)) {
		if (!library[JSON_STR_POLYS].isArray()) {
			ERR(path + JSON_STR_POLYS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& polygons = library[JSON_STR_POLYS];

		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_POLYS +
		    PFX + std::to_string(polygons.size()));
		// clang-format on

		int i = 1;
		for (const auto& polygon : polygons) {
			polygon_def pd {};
			// clang-format off
			std::string str = path + JSON_STR_POLYS +
					  LB + std::to_string(i) + RB;
			// clang-format on

			if (!polygon.isObject()) {
				ERR(str + PFX + ERR_OBJ);
				return false;
			}

			rc = parse_json_polygon(polygon, str + PFX, pd);
			if (!rc) {
				return false;
			}
			i++;

			ld.polygons.push_back((std::move(pd)));
		}
	}

	if (library.isMember(JSON_STR_MESHS)) {
		if (!library[JSON_STR_MESHS].isArray()) {
			ERR(path + JSON_STR_MESHS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& meshes = library[JSON_STR_MESHS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_MESHS +
		    PFX  + std::to_string(meshes.size()));
		// clang-format on

		int i = 1;
		for (const auto& mesh : meshes) {
			mesh_def md {};
			// clang-format off
			std::string str = path + JSON_STR_MESHS +
					  LB + std::to_string(i) + RB;
			// clang-format on

			if (!mesh.isObject()) {
				ERR(str + PFX + ERR_OBJ);
				return false;
			}

			rc = parse_json_mesh(mesh, str + PFX, md);
			if (!rc) {
				return false;
			}
			i++;

			ld.meshes.push_back((std::move(md)));
		}
	}

	if (library.isMember(JSON_STR_ELEMS)) {
		if (!library[JSON_STR_ELEMS].isArray()) {
			ERR(path + JSON_STR_ELEMS + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& elements = library[JSON_STR_ELEMS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_ELEMS +
		    PFX  + std::to_string(elements.size()));
		// clang-format on

		int i = 1;
		for (const auto& element : elements) {
			lib_elem_def ed {};
			// clang-format off
			std::string str = path + JSON_STR_ELEMS +
					  LB + std::to_string(i) + RB;
			// clang-format on

			if (!element.isObject()) {
				ERR(str + PFX + ERR_OBJ);
				return false;
			}

			rc = parse_json_lib_elem_def(element, str + PFX, ed);
			if (!rc) {
				return false;
			}
			i++;

			ld.lib_elem_defs.push_back(std::move(ed));
		}
	}

	// clang-format off
	DBG(path + STR_FNLN + SPC +
	    JSON_STR_POLYS + PFX + std::to_string(ld.polygons.size()));
	DBG(path + STR_FNLN + SPC +
	    JSON_STR_MESHS + PFX + std::to_string(ld.meshes.size()));
	DBG(path + STR_FNLN + SPC +
	    JSON_STR_ELEMS + PFX + std::to_string(ld.lib_elem_defs.size()));
	// clang-format on

	return true;
}

static bool parse_json_scene(const Json::Value& scene, const std::string path, scene_def& sd)
{
	bool rc;

	if (scene.isMember(JSON_STR_NAME)) {
		if (!scene[JSON_STR_NAME].isString()) {
			ERR(path + JSON_STR_NAME + PFX + ERR_STR);
			return false;
		}

		sd.name = scene[JSON_STR_NAME].asString();
		DBG(path + JSON_STR_NAME + PFX + sd.name);
	}

	if (scene.isMember(JSON_STR_ELEMS)) {
		std::string str = path + JSON_STR_ELEMS;
		// DBG(str);

		if (!scene[JSON_STR_ELEMS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		const Json::Value elements = scene[JSON_STR_ELEMS];
		// clang-format off
		DBG(path + STR_NUM + SPC + JSON_STR_ELEMS +
		    PFX + std::to_string(elements.size()));
		// clang-format on

		int i = 1;
		for (const auto& element : elements) {
			std::string err = str + LB + std::to_string(i) + RB;
			// DBG(str + PFX);
			if (!element.isObject()) {
				ERR(err + PFX + ERR_OBJ);
				return false;
			}

			scene_elem_def ed {};
			rc = parse_json_scene_elem_def(element, err + PFX, ed);
			if (!rc) {
				return false;
			}
			i++;

			sd.scene_elem_defs.push_back(std::move(ed));
		}

		// clang-format off
		DBG(path + STR_FNLN + SPC + JSON_STR_ELEMS + PFX +
		    std::to_string(sd.scene_elem_defs.size()));
		// clang-format on
	}

	if (scene.isMember(JSON_STR_ENV)) {
		std::string str = path + JSON_STR_ENV;

		if (!scene[JSON_STR_ENV].isObject()) {
			ERR(str + PFX + ERR_OBJ);
			return false;
		}

		const Json::Value& env = scene[JSON_STR_ENV];
		DBG(str + STR_NUM + SPC + STR_FLD + PFX + std::to_string(env.size()));

		environ_def ed {};
		rc = parse_json_environment(env, str + PFX, ed);
		if (!rc) {
			return false;
		}

		sd.env = std::move(ed);

		runtime_def rd {};
		rc = parse_json_runtime(env, str + PFX, rd);
		if (!rc) {
			ERR(STR_PRS << PFX << STR_SCN << PFX
			            << "parse_json_runtime()" << SPC << ERR_FAIL);
			return false;
		}

		sd.run = std::move(rd);
	}

	return true;
}

bool parse_json(std::ifstream& ifs, config_ns::AST& ast)
{
	Json::Value root;
	Json::CharReaderBuilder builder;
	std::string json_errs;
	config_ns::errors_t errs;
	bool rc;

	builder["collectComments"] = false;
	builder["rejectDupKeys"] = true;
	builder["allowTrailingCommas"] = false;

	DBG(STR_PRS);

	std::string path = STR_PRS + PFX;

	rc = parseFromStream(builder, ifs, &root, &json_errs);
	if (!rc) {
		// clang-format off
		ERR(path + STR_JSON + SPC + STR_PRSR +
		    ERR_FAILW + PFX + json_errs);
		// clang-format on
		return false;
	}

	if (!root.isObject()) {
		ERR(path + STR_JSON + PFX + STR_ROOT + ERR_OBJ);
		return false;
	}

	std::string str = path + JSON_STR_VER;

	// Mandatory. checked during parse
	if (!root.isMember(JSON_STR_VER)) {
		ERR(str + PFX + ERR_MISS);
		return false;
	}

	Json::Value& ver = root[JSON_STR_VER];
	if (!ver.isArray()) {
		ERR(str + PFX + ERR_ARR);
		return false;
	}

	uint32_t version[JSON_VER_FIELDS] = {JSON_VER_MAJ, JSON_VER_MIN, JSON_VER_REV};
	rc = check_json_version(ver, str + PFX, version);
	if (!rc) {
		return false;
	}

	// will be caught during validate
	if (root.isMember(JSON_STR_LIB)) {
		str = path + JSON_STR_LIB;
		DBG(str);

		if (!root[JSON_STR_LIB].isObject()) {
			ERR(str + PFX + ERR_OBJ);
			return false;
		}

		const Json::Value& library = root[JSON_STR_LIB];
		library_def ld {};

		rc = parse_json_library(library, str + PFX, ld);
		if (!rc) {
			return false;
		}

		ast.library_defs = ld;
	}

	// will be caught during validate
	if (root.isMember(JSON_STR_SCNS)) {
		str = path + JSON_STR_SCNS;
		DBG(str);

		if (!root[JSON_STR_SCNS].isArray()) {
			ERR(str + PFX + ERR_ARR);
			return false;
		}

		const Json::Value& scenes = root[JSON_STR_SCNS];
		// clang-format off
		DBG(str + PFX + STR_NUM + SPC + JSON_STR_SCNS +
		    PFX + std::to_string(scenes.size()));
		// clang-format on

		int i = 1;
		for (const auto& scene : scenes) {
			scene_def sd {};

			str += PFX + STR_SCN + LB + std::to_string(i) + RB;

			if (!scene.isObject()) {
				ERR(str + PFX + ERR_ARR);
				return false;
			}

			rc = parse_json_scene(scene, str + PFX, sd);
			if (!rc) {
				return false;
			}
			i++;

			ast.scene_defs.push_back(std::move(sd));
		}
	}

	// clang-format off
	DBG(path + STR_FNLN + SPC + JSON_STR_SCNS +
	    PFX + std::to_string(ast.scene_defs.size()));
	// clang-format on

	return true;
}

} // namespace config_ns
