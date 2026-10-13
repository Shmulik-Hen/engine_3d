#include <cctype>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define DEBUG_PRINTS
#include "common.h"
#include "config_json.h"
#include "utils.h"

namespace config_ns
{

const std::string JSON_STR_SCN = "scene";
const std::string JSON_STR_VEC = "vector";
const std::string JSON_STR_PAIR = "pair";
const std::string JSON_STR_POLY = "polygon";
const std::string JSON_STR_MESH = "mesh";
const std::string JSON_STR_ELEM = "element";

static bool check_version(const Json::Value& root, const uint32_t array[VERSION_NUM_FIELDS])
{
	if (!root.isMember(JSON_STR_VER)) {
		ERR(JSON_STR_VER << " is missing");
		return false;
	}

	Json::Value ver = root[JSON_STR_VER];
	if (!ver.isArray() || ver.size() != VERSION_NUM_FIELDS) {
		ERR(JSON_STR_VER << " is not an array or illegal size: required: "
		                 << VERSION_NUM_FIELDS << ", current: " << ver.size());
		return false;
	}

	bool match = true;
	for (uint32_t i = 0; i < VERSION_NUM_FIELDS; i++) {
		if (!ver[i].isUInt()) {
			ERR(JSON_STR_VER << "[" << i << "] is not an unsigned integer");
			return false;
		}

		if (ver[i].asUInt() != array[i]) {
			match = false;
			break;
		}
	}

	if (!match) {
		ERR(JSON_STR_VER << " unsupported version: required: "
		                 << array[0] << "." << array[1] << "." << array[2]
		                 << ", current: " << ver[0] << "." << ver[1] << "." << ver[2]);
		return false;
	}

	return true;
}

static bool parse_json_attrib(const Json::Value& attrib, unit array[NUM_ATTRIBUTES])
{
	if (!attrib.isArray() || attrib.size() != NUM_ATTRIBUTES) {
		ERR(JSON_STR_ATTR << " is not an array or illegal size: required: "
		                  << NUM_ATTRIBUTES << ", current: " << attrib.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_ATTRIBUTES; i++) {
		if (!attrib[i].isDouble()) {
			ERR(JSON_STR_ATTR << " must all be floating point numbers");
			return false;
		}

		array[i] = attrib[i].asFloat();
		DBG("parse_attrib:" << JSON_STR_ATTR << "[" << i << "]: " << array[i]);
	}

	return true;
}

static bool parse_json_vector(const Json::Value& vector, unit array[NUM_COORDS])
{
	if (!vector.isArray() || vector.size() != NUM_COORDS) {
		ERR(JSON_STR_VEC << " is not an array or illegal size: required: "
		                 << NUM_COORDS << ", current: " << vector.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_COORDS; i++) {
		if (!vector[i].isDouble()) {
			ERR(JSON_STR_VEC << " must all be floating point numbers");
			return false;
		}

		array[i] = vector[i].asFloat();
		DBG("parse_vector:" << JSON_STR_VEC << "[" << i << "]: " << array[i]);
	}

	return true;
}

static bool parse_json_pair(const Json::Value& pair, uint32_t array[NUM_POINTS])
{
	if (!pair.isArray() || pair.size() != NUM_POINTS) {
		ERR(JSON_STR_PAIR << " is not an array or illegal size: required: "
		                  << NUM_POINTS << ", current: " << pair.size());
		return false;
	}

	for (uint32_t i = 0; i < NUM_POINTS; i++) {
		if (!pair[0].isUInt()) {
			ERR(JSON_STR_PAIR << " must all be unsigned integers");
			return false;
		}

		array[i] = pair[i].asUInt();
		DBG("parse_pair:" << JSON_STR_PAIR << "[" << i << "]: " << array[i]);
	}

	return true;
}

static bool parse_json_polygon(const Json::Value& polygon, AST& ast)
{
	polygon_def pd {};
	unit coords[NUM_COORDS];
	bool rc;

	if (!polygon.isObject()) {
		ERR(JSON_STR_POLY << " is not an object");
		return false;
	}

	if (polygon.isMember(JSON_STR_NAME)) {
		if (polygon[JSON_STR_NAME].asString().empty()) {
			ERR(JSON_STR_POLY << ": " << JSON_STR_NAME << " is empty");
			return false;
		}

		pd.name = polygon[JSON_STR_NAME].asString();
		DBG("parse_polygon: " << JSON_STR_NAME << ": " << pd.name);
	}

	if (polygon.isMember(JSON_STR_COLOR)) {
		if (!polygon[JSON_STR_COLOR].isUInt()) {
			ERR(JSON_STR_POLY << ": " << JSON_STR_COLOR << " must be an unsigned integer");
			return false;
		}

		pd.color_index = polygon[JSON_STR_COLOR].asUInt();
		DBG("parse_polygon: " << JSON_STR_COLOR << ": " << pd.color_index);
	}

	if (polygon.isMember(JSON_STR_FORCE)) {
		if (!polygon[JSON_STR_FORCE].isBool()) {
			ERR(JSON_STR_POLY << ": " << JSON_STR_FORCE << " must be a boolean");
			return false;
		}

		pd.force = polygon[JSON_STR_FORCE].asBool();
		DBG("parse_polygon: " << JSON_STR_FORCE << ": " << pd.force);
	}

	if (polygon.isMember(JSON_STR_VECS)) {
		if (!polygon[JSON_STR_VECS].isArray()) {
			ERR(JSON_STR_POLY << ": " << JSON_STR_VECS << " is not an array");
			return false;
		}

		const Json::Value vectors = polygon[JSON_STR_VECS];
		const std::size_t size = vectors.size();
		DBG("parse_polygon: " << JSON_STR_VECS << ": size: " << size);

		for (uint32_t i = 0; i < size; i++) {
			const Json::Value& vector = vectors[i];
			rc = parse_json_vector(vector, coords);
			if (!rc) {
				ERR(JSON_STR_POLY << ": " << JSON_STR_VECS << "[" << i << "]: parse_json_vector() failed");
				return false;
			}

			vector_3 v(coords);
			pd.vertices.push_back(v);
		}
	}

	ast.polygons.push_back(std::move(pd));
	return true;
}

static bool parse_json_mesh(const Json::Value& mesh, AST& ast)
{
	mesh_def md {};

	if (!mesh.isObject()) {
		ERR(JSON_STR_MESH << " is not an object");
		return false;
	}

	if (mesh.isMember(JSON_STR_NAME)) {
		if (mesh[JSON_STR_NAME].asString().empty()) {
			ERR(JSON_STR_MESH << ": " << JSON_STR_NAME << " is empty");
			return false;
		}

		md.name = mesh[JSON_STR_NAME].asString();
		DBG("parse_mesh: " << JSON_STR_NAME << ": " << md.name);
	}

	if (mesh.isMember(JSON_STR_POLYS)) {
		if (!mesh[JSON_STR_POLYS].isArray()) {
			ERR(JSON_STR_MESH << ": " << JSON_STR_POLYS << " is not an array");
			return false;
		}

		const Json::Value poly_names = mesh[JSON_STR_POLYS];
		const std::size_t size = poly_names.size();
		DBG("parse_mesh: " << JSON_STR_POLYS << " size: " << size);

		for (uint32_t i = 0; i < size; i++) {
			const Json::Value poly = poly_names[i];
			if (!poly.isString()) {
				ERR(JSON_STR_MESH << ": " << JSON_STR_POLYS << "[" << i << "]: " << " is not a string");
				return false;
			}

			const std::string name = poly_names[i].asString();
			if (name.empty()) {
				ERR(JSON_STR_MESH << ": " << JSON_STR_POLYS << "[" << i << "]: " << " is empty");
				return false;
			}

			md.polygons.push_back(name);
			DBG("parse_mesh: " << JSON_STR_POLYS << "[" << i << "]: " << name);
		}
	}

	ast.meshes.push_back(std::move(md));
	return true;
}

static bool parse_json_element_def(const Json::Value& element, AST& ast)
{
	element_def ed {};

	if (!element.isObject()) {
		ERR(JSON_STR_ELEM << " is not an object");
		return false;
	}

	if (element.isMember(JSON_STR_NAME)) {
		if (element[JSON_STR_NAME].asString().empty()) {
			ERR(JSON_STR_ELEM << ": " << JSON_STR_NAME << " is empty");
			return false;
		}

		ed.name = element[JSON_STR_NAME].asString();
		DBG("parse_elem_def: " << JSON_STR_NAME << ": " << ed.name);
	}

	if (element.isMember(JSON_STR_MESH)) {
		if (element[JSON_STR_MESH].asString().empty()) {
			ERR(JSON_STR_ELEM << ": " << JSON_STR_MESH << " is empty");
			return false;
		}

		ed.mesh = element[JSON_STR_MESH].asString();
		DBG("parse_elem_def: " << JSON_STR_MESH << ": " << ed.mesh);
	}

	ast.element_defs.push_back(std::move(ed));
	return true;
}

static bool parse_json_element_ref(const Json::Value& element, AST& ast)
{
	element_ref er;
	bool rc;

	if (!element.isObject()) {
		ERR(JSON_STR_ELEM << " is not an object");
		return false;
	}

	if (element.isMember(JSON_STR_NAME)) {
		if (element[JSON_STR_NAME].asString().empty()) {
			ERR(JSON_STR_ELEM << ": " << JSON_STR_NAME << " is empty");
			return false;
		}

		er.name = element[JSON_STR_NAME].asString();
		DBG("parse_elem_ref: " << JSON_STR_NAME << ": " << er.name);
	}

	if (element.isMember(JSON_STR_PAREN)) {
		if (element[JSON_STR_PAREN].isNull()) {
			DBG("parse_elem_ref: " << JSON_STR_PAREN << " is null");
			er.parent = std::nullopt;
		}
		else {
			if (element[JSON_STR_PAREN].asString().empty()) {
				ERR(JSON_STR_ELEM << ": " << JSON_STR_PAREN << " is empty");
				return false;
			}

			DBG("parse_elem_ref: " << JSON_STR_PAREN << ": " << element[JSON_STR_PAREN].asString());
			er.parent = std::optional<std::string> {element[JSON_STR_PAREN].asString()};
		}
	}

	if (element.isMember(JSON_STR_ACT)) {
		if (!element[JSON_STR_ACT].isBool()) {
			ERR(JSON_STR_ELEM << ": " << JSON_STR_ACT << " is not a boolean");
			return false;
		}

		er.active = element[JSON_STR_ACT].asBool();
		DBG("parse_elem_ref: " << JSON_STR_ACT << ": " << er.active);
	}

	if (element.isMember(JSON_STR_REF)) {
		if (element[JSON_STR_REF].asString().empty()) {
			ERR(JSON_STR_ELEM << ": " << JSON_STR_REF << " is empty");
			return false;
		}

		DBG("parse_elem_ref: " << JSON_STR_REF << ": " << element[JSON_STR_REF].asString());
		er.ref = std::optional<std::string> {element[JSON_STR_REF].asString()};
	}

	if (element.isMember(JSON_STR_ATTR)) {
		unit array[NUM_ATTRIBUTES];

		rc = parse_json_attrib(element[JSON_STR_ATTR], array);
		if (!rc) {
			ERR(JSON_STR_ELEM << ": " << JSON_STR_ATTR << ": parse_json_attrib() failed");
			return false;
		}

		attrib att(array);
		er.ini_att = att;
		att.print();
	}

	if (element.isMember(JSON_STR_MOTN)) {
		unit array[NUM_ATTRIBUTES];

		rc = parse_json_attrib(element[JSON_STR_MOTN], array);
		if (!rc) {
			ERR(JSON_STR_ELEM << ": " << JSON_STR_MOTN << ": parse_json_attrib() failed");
			return false;
		}

		attrib att(array);
		er.run_att = att;
		att.print();
	}

	ast.element_refs.push_back(std::move(er));
	return true;
}

static bool parse_json_library(const Json::Value& root, AST& ast)
{
	bool rc;

	if (!root.isMember(JSON_STR_LIB)) {
		// silently fail - will be caught during validaton
		return true;
	}

	if (!root[JSON_STR_LIB].isObject()) {
		ERR(JSON_STR_LIB << " is not an object");
		return false;
	}
	const Json::Value library = root[JSON_STR_LIB];

	if (library.isMember(JSON_STR_POLYS)) {
		if (!library[JSON_STR_POLYS].isArray()) {
			ERR(JSON_STR_LIB << ": " << JSON_STR_POLYS << " is not an array");
			return false;
		}

		const Json::Value polygons = library[JSON_STR_POLYS];
		const std::size_t size = polygons.size();
		DBG("parse_lib: " << JSON_STR_POLYS << " size: " << size);

		for (uint32_t i = 0; i < size; i++) {
			const Json::Value polygon = polygons[i];
			rc = parse_json_polygon(polygon, ast);
			if (!rc) {
				ERR(JSON_STR_LIB << ": " << JSON_STR_POLYS << "[" << i << "]: parse_json_polygon() failed");
				return false;
			}
		}
	}

	if (library.isMember(JSON_STR_MESHS)) {
		if (!library[JSON_STR_MESHS].isArray()) {
			ERR(JSON_STR_LIB << ": " << JSON_STR_MESHS << " is not an array");
			return false;
		}

		const Json::Value meshes = library[JSON_STR_MESHS];
		const std::size_t size = meshes.size();
		DBG("parse_lib: " << JSON_STR_MESHS << " size: " << size);

		for (uint32_t i = 0; i < size; i++) {
			const Json::Value mesh = meshes[i];
			rc = parse_json_mesh(mesh, ast);
			if (!rc) {
				ERR(JSON_STR_LIB << ": " << JSON_STR_MESHS << "[" << i << "]: parse_json_mesh() failed");
				return false;
			}
		}
	}

	if (library.isMember(JSON_STR_ELEMS)) {
		if (!library[JSON_STR_ELEMS].isArray()) {
			ERR(JSON_STR_LIB << ": " << JSON_STR_ELEMS << " is not an array");
			return false;
		}

		const Json::Value elements = library[JSON_STR_ELEMS];
		const std::size_t size = elements.size();
		DBG("parse_lib: " << JSON_STR_ELEMS << " size: " << size);

		for (uint32_t i = 0; i < size; i++) {
			const Json::Value element = elements[i];
			rc = parse_json_element_def(element, ast);
			if (!rc) {
				ERR(JSON_STR_LIB << ": " << JSON_STR_MESHS << "[" << i << "]: parse_json_element_ref() failed");
				return false;
			}
		}
	}

	DBG("parse_library: num " << JSON_STR_POLYS << ": " << (int)ast.polygons.size());
	DBG("parse_library: num " << JSON_STR_MESHS << ": " << (int)ast.meshes.size());
	DBG("parse_library: num " << JSON_STR_ELEMS << ": " << (int)ast.element_defs.size());

	return true;
}

static bool parse_json_environment(const Json::Value& scene, scene_def& sd)
{
	bool rc;
	unit coords[NUM_COORDS];
	uint32_t points[NUM_POINTS];

	if (!scene.isMember(JSON_STR_ENV)) {
		// silently fail - will be caught during validaton
		return true;
	}

	if (!scene[JSON_STR_ENV].isObject()) {
		ERR(JSON_STR_ENV << " is not an object");
		return false;
	}

	const Json::Value env = scene[JSON_STR_ENV];
	DBG("parse_env: num fields: " << env.size());

	if (env.isMember(JSON_STR_COLOR)) {
		DBG("parse_env: found: " << JSON_STR_COLOR);
		if (!env[JSON_STR_COLOR].isUInt()) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_COLOR << " must be an unsigned integer");
			return false;
		}

		DBG("parse_env: " << JSON_STR_COLOR << ": " << env[JSON_STR_COLOR].asUInt());
		sd.env.color_idx = env[JSON_STR_COLOR].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_TYPE)) {
		DBG("parse_env: found: " << JSON_STR_LIGHT_TYPE);
		if (!env[JSON_STR_LIGHT_TYPE].isUInt()) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_LIGHT_TYPE << " must be an unsigned integer");
			return false;
		}

		DBG("parse_env: " << JSON_STR_LIGHT_TYPE << ": " << env[JSON_STR_LIGHT_TYPE].asUInt());
		sd.env.light_type = env[JSON_STR_LIGHT_TYPE].asUInt();
	}

	if (env.isMember(JSON_STR_LIGHT_DIR)) {
		DBG("parse_env: found: " << JSON_STR_LIGHT_DIR);
		rc = parse_json_vector(env[JSON_STR_LIGHT_DIR], coords);
		if (!rc) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_LIGHT_DIR << "parse_json_vector() failed");
			return false;
		}

		vector_3 v(coords);
		sd.env.light_direction = v;
		v.print();
	}

	if (env.isMember(JSON_STR_LIGHT_POS)) {
		DBG("parse_env: found: " << JSON_STR_LIGHT_POS);
		rc = parse_json_vector(env[JSON_STR_LIGHT_POS], coords);
		if (!rc) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_LIGHT_POS << "parse_json_vector() failed");
			return false;
		}

		vector_3 v(coords);
		sd.env.light_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_CAM_POS)) {
		DBG("parse_env: found: " << JSON_STR_CAM_POS);
		rc = parse_json_vector(env[JSON_STR_CAM_POS], coords);
		if (!rc) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_CAM_POS << "parse_json_vector() failed");
			return false;
		}

		vector_3 v(coords);
		sd.env.camera_position = v;
		v.print();
	}

	if (env.isMember(JSON_STR_FOCAL_LEN)) {
		DBG("parse_env: found: " << JSON_STR_FOCAL_LEN);
		if (!env[JSON_STR_FOCAL_LEN].isDouble()) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_FOCAL_LEN << " must be a floating point number");
			return false;
		}

		DBG("parse_env: " << JSON_STR_FOCAL_LEN << ": " << env[JSON_STR_FOCAL_LEN].asFloat());
		sd.env.focal_len = env[JSON_STR_FOCAL_LEN].asFloat();
	}

	if (env.isMember(JSON_STR_NEAR_EPS)) {
		DBG("parse_env: found: " << JSON_STR_NEAR_EPS);
		if (!env[JSON_STR_NEAR_EPS].isDouble()) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_NEAR_EPS << " must be a floating point number");
			return false;
		}

		DBG("parse_env: " << JSON_STR_NEAR_EPS << ": " << env[JSON_STR_NEAR_EPS].asFloat());
		sd.env.near_eps = env[JSON_STR_NEAR_EPS].asFloat();
	}

	if (env.isMember(JSON_STR_LOOPS)) {
		DBG("parse_env: found: " << JSON_STR_LOOPS);
		if (!env[JSON_STR_LOOPS].isInt()) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_LOOPS << " must be an integer");
			return false;
		}

		DBG("parse_env: " << JSON_STR_LOOPS << ": " << env[JSON_STR_LOOPS].asInt());
		sd.run.loops = env[JSON_STR_LOOPS].asInt();
	}

	if (env.isMember(JSON_STR_LOOP_DELAY)) {
		DBG("parse_env: found: " << JSON_STR_LOOP_DELAY);
		if (!env[JSON_STR_LOOP_DELAY].isUInt()) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_LOOP_DELAY << " must be an unsigned integer");
			return false;
		}

		DBG("parse_env: " << JSON_STR_LOOP_DELAY << ": " << env[JSON_STR_LOOP_DELAY].asUInt());
		sd.run.loop_delay = env[JSON_STR_LOOP_DELAY].asUInt();
	}

	if (env.isMember(JSON_STR_MIN_POS)) {
		DBG("parse_env: found: " << JSON_STR_MIN_POS);
		rc = parse_json_pair(env[JSON_STR_MIN_POS], points);
		if (!rc) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_MIN_POS << "parse_json_pair() failed");
			return false;
		}

		DBG("parse_env: " << JSON_STR_MIN_POS << "(" << points[0] << ", " << points[1] << ")");
		sd.env.min_pos = config_ns::point_def {points[0], points[1]};
	}

	if (env.isMember(JSON_STR_MAX_POS)) {
		DBG("parse_env: found: " << JSON_STR_MAX_POS);
		rc = parse_json_pair(env[JSON_STR_MAX_POS], points);
		if (!rc) {
			ERR(JSON_STR_ENV << ": " << JSON_STR_MAX_POS << "parse_json_pair() failed");
			return false;
		}

		DBG("parse_env: " << JSON_STR_MAX_POS << "(" << points[0] << ", " << points[1] << ")");
		sd.env.max_pos = config_ns::point_def {points[0], points[1]};
	}

	return true;
}

static bool parse_json_scene(const Json::Value& scene, AST& ast)
{
	scene_def sd;
	bool rc;

	if (!scene.isObject()) {
		ERR(JSON_STR_SCN << " is not an object");
		return false;
	}

	if (scene.isMember(JSON_STR_NAME)) {
		if (scene[JSON_STR_NAME].asString().empty()) {
			ERR(JSON_STR_SCN << ": " << JSON_STR_NAME << " is empty");
			return false;
		}

		sd.name = scene[JSON_STR_NAME].asString();
		DBG("parse_elem_ref: " << JSON_STR_NAME << ": " << sd.name);
	}

	if (scene.isMember(JSON_STR_ELEMS)) {
		if (!scene[JSON_STR_ELEMS].isArray()) {
			ERR(JSON_STR_SCN << ": " << JSON_STR_ELEMS << " is not an array");
			return false;
		}

		const Json::Value elements = scene[JSON_STR_ELEMS];
		const std::size_t size = elements.size();
		DBG("parse_elem_ref: " << JSON_STR_ELEMS << " size: " << size);

		for (uint32_t i = 0; i < size; i++) {
			const Json::Value element = elements[i];
			rc = parse_json_element_ref(element, ast);
			if (!rc) {
				ERR(JSON_STR_SCN << ": " << JSON_STR_ELEMS << "[" << i << "]: parse_json_element_ref() failed");
				return false;
			}
		}
	}

	if (scene.isMember(JSON_STR_ENV)) {
		rc = parse_json_environment(scene, sd);
		if (!rc) {
			ERR(JSON_STR_SCN << ": " << "parse_json_environment() failed");
			return false;
		}
	}

	ast.scene_defs.push_back(std::move(sd));
	return true;
}

static bool parse_json_scenes(const Json::Value& root, AST& ast)
{
	bool rc;

	if (!root.isMember(JSON_STR_SCNS)) {
		// silently fail - will be caught during validaton
		return true;
	}

	if (!root[JSON_STR_SCNS].isArray()) {
		ERR(JSON_STR_SCNS << " is not an array");
		return false;
	}

	const Json::Value scenes = root[JSON_STR_SCNS];
	const std::size_t size = scenes.size();
	DBG("parse_scenes: " << JSON_STR_SCNS << " size: " << size);

	for (uint32_t i = 0; i < size; i++) {
		const Json::Value scene = scenes[i];
		rc = parse_json_scene(scene, ast);
		if (!rc) {
			ERR(JSON_STR_SCNS << "[" << i << "]: parse_json_scene() failed");
			return false;
		}
	}

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

	DBG("parse_json: file name: " << filename);
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

	uint32_t version[VERSION_NUM_FIELDS] = {MAJOR, MINOR, REVISION};
	rc = check_version(root, version);
	if (!rc) {
		sys_error("unsupported config version in: ", filename.c_str());
	}

	rc = parse_json_library(root, ast);
	if (!rc) {
		sys_error("parse_json_library() failed in: ", filename.c_str());
	}

	rc = parse_json_scenes(root, ast);
	if (!rc) {
		sys_error("parse_json_scenes() failed in: ", filename.c_str());
	}

	return ast;
}

} // namespace config_ns
