#include <stdexcept>
#include <string>
#include <cctype>

// #define DEBUG_PRINTS
#include "common.h"
#include "config_json.h"
#include "utils.h"

namespace config_ns
{

bool parse_json_attrib(unit attribs[NUM_ATTRIBUTES], const Json::Value& attrib, const unsigned int idx [[maybe_unused]])
{
	DBG("parse_attrib: " << idx << ", num attribs: " << attrib.size());
	if (attrib.size() != NUM_ATTRIBUTES) {
		ERR("parse_attrib: bad attribs number");
		return false;
	}

	for (unsigned int i = 0; i < attrib.size(); i++) {
		DBG("parse_attrib: " << idx << ", " << i << ", attrib: " << i << ", val: " << attrib[i]);
		attribs[i] = attrib[i].asFloat();
	}

	return true;
}

bool parse_json_vector(unit coords[NUM_COORDS], const Json::Value& point, const unsigned int idx [[maybe_unused]])
{
	DBG("parse_vector: " << idx << ", num points: " << point.size());
	if (point.size() != NUM_COORDS) {
		ERR("parse_vector: idx: " << idx << "bad coord number");
		return false;
	}

	for (unsigned int i = 0; i < point.size(); i++) {
		DBG("parse_vector: " << idx << ", " << i << ", coord: " << i << ", val: " << point[i]);
		coords[i] = point[i].asFloat();
	}

	return true;
}

bool parse_json_pair(int points[2], const Json::Value& point, const unsigned int idx [[maybe_unused]])
{
	DBG("parse_pair: " << idx << ", num points: " << point.size());
	points[0] = point[0].asInt();
	points[1] = point[1].asInt();

	return true;
}

bool parse_json_polygon(AST& ast, const Json::Value& polygon, const unsigned int idx [[maybe_unused]])
{
	unit coords[NUM_COORDS];
	polygon_def pd {};
	bool rc;

	DBG("parse_polygon: " << idx << ", name: " << polygon["name"]);
	pd.name = polygon["name"].asString();

	DBG("parse_polygon: " << idx << ", color_idx: " << polygon["color_idx"]);
	pd.color_index = polygon["color_idx"].asInt();

	DBG("parse_polygon: " << idx << ", force: " << polygon["force"]);
	pd.force = polygon["force"].asBool();

	const Json::Value objects = polygon["objects"];
	DBG("parse_polygon: " << idx << ", num objects: " << objects.size());

	for (unsigned int i = 0; i < objects.size(); i++) {
		const Json::Value object = objects[i];
		const Json::Value type = object["type"];
		DBG("parse_polygon: " << idx << ", " << i << ", type: " << type.asString());
		if (type == "vector") {
			const Json::Value& points = object["points"];
			DBG("parse_polygon: " << idx << ", " << i << ", num points: " << points.size());
			if (points.size() < MIN_VECTORS) {
				ERR("parse_polygon: " << idx << ", " << i << " too few vectors: " << points.size() << " required at least: " << MIN_VECTORS);
				return false;
			}
			for (unsigned int j = 0; j < points.size(); j++) {
				const Json::Value& point = points[j];
				rc = parse_json_vector(coords, point, j);
				if (!rc) {
					return false;
				}
				pd.points.push_back(coords);
			}
		}
		else if (polygon.isMember("normal")) {
			const Json::Value& points = object["points"];
			DBG("parse_polygon: " << idx << ", " << i << ", num coords: " << points.size());
			const Json::Value& point = points[0];
			rc = parse_json_vector(coords, point, i);
			if (!rc) {
				return false;
			}

			vector_3 v(coords);
			pd.normal_cfg = v;
		}
		else {
			WARN("parse_polygon: unsupported object type" << type.asString());
		}
	}

	ast.polygons.push_back(std::move(pd));
	return true;
}

bool parse_json_element(AST& ast, const Json::Value& element, const unsigned int idx [[maybe_unused]])
{
	unit atts[NUM_ATTRIBUTES] = {0, 0, 0, 0, 0, 0, 1};
	element_def ed {};
	bool rc;

	DBG("parse_element: " << idx << ", name: " << element["name"]);
	ed.name = element["name"].asString();

	DBG("parse_element: " << idx << ", parrent: " << element["parrent"]);
	ed.parent = element["parrent"].asString();

	DBG("parse_element: " << idx << ", active: " << element["active"]);
	ed.active = element["active"].asBool();

	const Json::Value objects = element["objects"];
	DBG("parse_element: " << idx << ", num objects: " << objects.size());
	for (unsigned int i = 0; i < objects.size(); i++) {
		const Json::Value object = objects[i];
		const Json::Value type = object["type"];
		DBG("parse_element: " << idx << ", " << i << ", type: " << type.asString());
		if (type == "attribute") {
			if (object.isMember("attrib")) {
				const Json::Value attrib = object["attrib"];
				DBG("parse ini attribs");
				rc = parse_json_attrib(atts, attrib, i);
				if (!rc) {
					return false;
				}
				ed.ini_att = atts;
				ed.ini_att.print();
			}
			else if (object.isMember("motion")) {
				DBG("parse motion attribs");
				const Json::Value motion = object["motion"];
				rc = parse_json_attrib(atts, motion, i);
				if (!rc) {
					return false;
				}
				ed.run_att = atts;
				ed.run_att.value().print();
			}
			else {
				WARN("parse_element: unsupported attrib type" << type.asString());
			}
		}
		else {
			WARN("parse_element: unsupported object type" << type.asString());
		}
	}

	const Json::Value polygons = element["polygons"];
	DBG("parse_element: " << idx << ", num polygons: " << polygons.size());
	for (unsigned int i = 0; i < polygons.size(); i++) {
		DBG("parse_element: " << idx << ", polygon: " << i << ", name: " << polygons[i]);
		ed.polygons.emplace_back(polygons[i].asString());
	}

	ast.elements.push_back(std::move(ed));
	return true;
}

bool parse_json_environment(AST& ast, const Json::Value& object, const unsigned int idx [[maybe_unused]])
{
	unit coords[NUM_COORDS];
	int points[2];
	Json::Value val;
	bool rc;

	DBG("parse_env: environment:");
	if (object.isMember("color_idx")) {
		DBG("parse_env: color_idx:");
		val = object["color_idx"];
		ast.env.color_idx = val.asInt();
	}

	if (object.isMember("light_type")) {
		DBG("parse_env: light_type:");
		val = object["light_type"];
		ast.env.light_type = val.asInt();
	}

	if (object.isMember("light_direction")) {
		DBG("parse_env: light_direction:");
		val = object["light_direction"];
		rc = parse_json_vector(coords, val, idx);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ast.env.light_direction = v;
	}

	if (object.isMember("light_position")) {
		DBG("parse_env: light_position:");
		val = object["light_position"];
		rc = parse_json_vector(coords, val, idx);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ast.env.light_position = v;
	}

	if (object.isMember("camera_position")) {
		DBG("parse_env: camera_position:");
		val = object["camera_position"];
		rc = parse_json_vector(coords, val, idx);
		if (!rc) {
			return false;
		}

		vector_3 v(coords);
		ast.env.camera_position = v;
	}

	if (object.isMember("focal_len")) {
		DBG("parse_env: focal_len:");
		val = object["focal_len"];
		ast.env.focal_len = val.asFloat();
	}

	if (object.isMember("near_eps")) {
		val = object["near_eps"];
		ast.env.near_eps = val.asFloat();
	}

	if (object.isMember("min_pos")) {
		DBG("parse_env: min_pos:");
		val = object["min_pos"];
		rc = parse_json_pair(points, val, idx);
		if (!rc) {
			return false;
		}

		ast.env.min_pos = config_ns::point_def {points[0], points[1]};
	}

	if (object.isMember("max_pos")) {
		DBG("parse_env: max_pos:");
		val = object["max_pos"];
		rc = parse_json_pair(points, val, idx);
		if (!rc) {
			return false;
		}

		ast.env.max_pos = config_ns::point_def {points[0], points[1]};
	}

	if (object.isMember("loops")) {
		DBG("parse_env: loops:");
		val = object["loops"];
		ast.run.loops = val.asInt();
	}

	if (object.isMember("loop_delay")) {
		DBG("parse_env: loop_delay:");
		val = object["loop_delay"];
		ast.run.loop_delay = val.asInt();
	}

	return true;
}

bool parse_json_objects(AST& ast, const Json::Value& conf, const unsigned int idx [[maybe_unused]])
{
	bool rc;

	const Json::Value objects = conf["objects"];
	DBG("parse_object: num objects: " << objects.size());
	for (unsigned int i = 0; i < objects.size(); i++) {
		const Json::Value object = objects[i];
		const Json::Value type = object["type"];
		DBG("parse_object: " << idx << ", " << i << " , type: " << type.asString());

		if (type == "polygon") {
			rc = parse_json_polygon(ast, object, i);
			if (!rc) {
				return false;
			}
		}
		else if (type == "element") {
			rc = parse_json_element(ast, object, i);
			if (!rc) {
				return false;
			}
		}
		else if (type == "environment") {
			rc = parse_json_environment(ast, object, i);
			if (!rc) {
				return false;
			}
		}
		else {
			ERR("parse_object: unsupported object type" << type.asString());
			return false;
		}
	}

	DBG("parse_object: num polygons: " << (int)ast.polygons.size());
	DBG("parse_object: num elements: " << (int)ast.elements.size());
	return true;
}

AST parse_json(const std::string& filename, const std::string& conf_name)
{
	AST ast;
	bool rc;
	std::ifstream ifs;
	Json::Value root;
	Json::CharReaderBuilder builder;
	JSONCPP_STRING errs;
	bool found = false;

	ifs.open(filename.c_str(), std::ios::in);
	if (!ifs) {
		sys_error("parse_json: file not found:", filename.c_str());
	}

	ifs.unsetf(std::ios::skipws);

	builder["collectComments"] = false;
	rc = parseFromStream(builder, ifs, &root, &errs);
	ifs.close();
	if (!rc) {
		sys_error("parse_json: ", errs.c_str());
	}

	// std::cout << root << std::endl;
	const Json::Value configurations = root["configurations"];

	DBG("parse_json: conf_name: " << conf_name);
	DBG("parse_json: num configurations: " << configurations.size());
	for (unsigned int i = 0; i < configurations.size(); i++) {
		const Json::Value conf = configurations[i];
		DBG("parse_json: configuration: " << i << ", " << conf["name"]);
		if (conf["name"] == conf_name) {
			found = true;
			rc = parse_json_objects(ast, conf, i);
			if (!rc) {
				sys_error("parse_json: configuration parse failed: ", conf_name.c_str());
			}
		}
	}

	if (!found) {
		sys_error("parse_json: configuration not found: ", conf_name.c_str());
	}

	return ast;
}

} // namespace config_ns
