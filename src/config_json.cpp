#include <stdexcept>
#include <string>
#include <cctype>

// #define DEBUG_PRINTS
#include "common.h"
#include "config_json.h"
#include "utils.h"

namespace config_ns
{

bool parse_polygon_json(polygon_def& pd, const Json::Value& polygon, const unsigned int idx [[maybe_unused]])
{
	unit coords[NUM_COORDS];

	DBG("parse_polygon: " << idx << ", name: " << polygon["name"]);
	pd.name = polygon["name"].asString();

	DBG("parse_polygon: " << idx << ", color_idx: " << polygon["color_idx"]);
	pd.color_index = polygon["color_idx"].asInt();

	DBG("parse_polygon: " << idx << ", force: " << polygon["force"]);
	pd.force = polygon["force"].asBool();

	const Json::Value points = polygon["points"];
	DBG("parse_polygon: " << idx << ", num points: " << points.size());
	if (points.size() < MIN_VECTORS) {
		ERR("parse_polygon: bad vertex number");
		return false;
	}

	for (unsigned int k = 0; k < points.size(); k++) {
		const Json::Value point = points[k];
		DBG("parse_polygon: " << idx << ", point: " << k << ", num coords: " << point.size());
		if (point.size() != NUM_COORDS) {
			ERR("parse_polygon: bad coord number");
			return false;
		}

		for (unsigned int l = 0; l < point.size(); l++) {
			DBG("parse_polygon: " << idx << ", coord: " << l << ", val: " << point[l]);
			coords[l] = point[l].asFloat();
		}

		vector_3 v(coords);
		pd.points.push_back(v);
	}

	const Json::Value normal = polygon["normal"];
	DBG("parse_polygon: " << idx << ", normal: num coords: " << normal.size());
	if (normal.size() != NUM_COORDS) {
		ERR("parse_polygon: coord number");
		return false;
	}

	for (unsigned int k = 0; k < normal.size(); k++) {
		DBG("parse_polygon: " << idx << ", coord: " << k << ", val: " << normal[k]);
		coords[k] = normal[k].asFloat();
	}

	vector_3 n(coords);
	pd.normal_cfg = n;

	return true;
}

bool parse_element_json(element_def& ed, const Json::Value& element, const unsigned int idx [[maybe_unused]])
{
	unit atts[NUM_ATTRIBUTES];

	DBG("parse_element: " << idx << ", name: " << element["name"]);
	ed.name = element["name"].asString();

	DBG("parse_element: " << idx << ", parrent: " << element["parrent"]);
	ed.parent = element["parrent"].asString();

	DBG("parse_element: " << idx << ", active: " << element["active"]);
	ed.active = element["active"].asBool();

	const Json::Value attributes = element["attributes"];
	DBG("parse_element: " << idx << ", num attrib: " << attributes.size());
	if (attributes.size() != NUM_ATTRIBUTES) {
		ERR("parse_element: bad attributes number");
		return false;
	}

	for (unsigned int k = 0; k < attributes.size(); k++) {
		DBG("parse_element: " << idx << ", att: " << k << ", val: " << attributes[k]);
		atts[k] = attributes[k].asFloat();
	}
	attrib att(atts);
	ed.att = att;

	const Json::Value polygons = element["polygons"];
	DBG("parse_element: " << idx << ", num polygons: " << polygons.size());
	for (unsigned int k = 0; k < polygons.size(); k++) {
		DBG("parse_element: " << idx << ", polygon: " << k << ", name: " << polygons[k]);
		ed.polygons.emplace_back(polygons[k].asString());
	}

	return true;
}

document parse_json(const std::string& filename, const std::string& config)
{
	document doc;
	polygon_def pd;
	element_def ed;
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

	DBG("parse_json: config: " << config);
	DBG("parse_json: num configurations: " << configurations.size());
	for (unsigned int i = 0; i < configurations.size(); i++) {
		const Json::Value conf = configurations[i];
		DBG("parse_json: configuration: " << i << ", " << conf["name"]);
		if (conf["name"] == config) {
			found = true;
			const Json::Value polygons = conf["polygons"];
			DBG("parse_json: num polygons: " << polygons.size());
			for (unsigned int j = 0; j < polygons.size(); j++) {
				const Json::Value poly = polygons[j];
				rc = parse_polygon_json(pd, poly, j);
				if (!rc) {
					sys_error("parse_polygon_json: failed to parse");
				}
				doc.polygons.push_back(std::move(pd));
			}

			const Json::Value elements = conf["elements"];
			DBG("parse_json: num elements: " << elements.size());
			for (unsigned j = 0; j < elements.size(); j++) {
				const Json::Value elem = elements[j];
				rc = parse_element_json(ed, elem, j);
				if (!rc) {
					sys_error("parse_element_json: failed to parse");
				}
				doc.elements.push_back(std::move(ed));
			}
		}
	}

	if (!found) {
		sys_error("parse_json: configuration not found: ", config.c_str());
	}

	return doc;
}

} // namespace config_ns
