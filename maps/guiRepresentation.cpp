#include "guiRepresentation.h"

#include "mapData.h"

static bool isRoad(std::string routeName) {
	if (routeName == "footway" || routeName == "path" || routeName == "pedestrian"
		|| routeName == "steps" || routeName == "elevator" || routeName == "platform"
		|| routeName == "bridleway") {
		return false;
	}
	else if (routeName == "motorway" || routeName == "primary") {
		return true;
	}
	else if (routeName == "motorway_link" || routeName == "primary_link") {
		return true;
	}
	else if (routeName == "unclassified" || routeName == "tertiary" || routeName == "secondary"
		|| routeName == "secondary_link" || routeName == "tertiary_link" || routeName == "residential") {
		return true;
	}
	else if (routeName == "track" || routeName == "construction" || routeName == "service") {
		return false;
	}
	else if (routeName == "cycleway") {
		return false;
	}
	else if (routeName == "trunk") {
		return true;
	}
	else if (routeName == "trunk_link") {
		return true;
	}
	else if (routeName == "living_street") {
		return true;
	}
	return false;
}

static bool isMainRoad(std::string routeName) {
	if (routeName == "footway" || routeName == "path" || routeName == "pedestrian"
		|| routeName == "steps" || routeName == "elevator" || routeName == "platform"
		|| routeName == "bridleway") {
		return false;
	}
	else if (routeName == "motorway" || routeName == "primary") {
		return true;
	}
	else if (routeName == "motorway_link" || routeName == "primary_link") {
		return false;
	}
	else if (routeName == "unclassified" || routeName == "tertiary" || routeName == "secondary"
		|| routeName == "secondary_link" || routeName == "tertiary_link" || routeName == "residential") {
		return false;
	}
	else if (routeName == "track" || routeName == "construction" || routeName == "service") {
		return false;
	}
	else if (routeName == "cycleway") {
		return false;
	}
	else if (routeName == "trunk") {
		return true;
	}
	else if (routeName == "trunk_link") {
		return false;
	}
	else if (routeName == "living_street") {
		return false;
	}
	return false;
}

void GuiRepresentation::Area::insertNode(const id_t id) {
	_nodes.push_back(id);
}

void GuiRepresentation::Area::insertWay(const id_t id) {
	_ways.push_back(id);
}

const std::vector<id_t>& GuiRepresentation::Area::nodes() const {
	return _nodes;
}

const std::vector<id_t>& GuiRepresentation::Area::ways() const {
	return _ways;
}

GuiRepresentation::GuiRepresentation()
{
	// init grid
	for (size_t scale = 0; scale < SCALES_NUM; scale++) {
		_map.push_back({});
		const size_t areasNum2 = 1 << (scale << 1);
		for (size_t i = 0; i < areasNum2; i++)
			_map.back().push_back(Area{});
	}

	// assign nodes to the grid positions
	for (const Node& node : MapData::instance().nodes())
        for (const auto& areaInfo : ScaleAreaInformation::areaInfos(node))
			_map[areaInfo.scale][areaInfo.areaIndex()].insertNode(node.id());

	// assign ways to the grid positions: secondary roads et al. only appear when we zoom in.
	for (const Way& way : MapData::instance().ways()) {
		if (!way.hasTag("highway"))
			continue;

        for (const auto& areaInfo : ScaleAreaInformation::areaInfos(way)) {
			if (areaInfo.scale < 2 && !isMainRoad(way.tagValue("highway")))
				continue;

			if (areaInfo.scale < 4 && !isRoad(way.tagValue("highway")))
				continue;

			_map[areaInfo.scale][areaInfo.areaIndex()].insertWay(way.id());
		}
	}
}

const std::vector<id_t>& GuiRepresentation::waysToVisualize(const Bounds& bounds) const {
	const auto info = bounds.scaleAndCoords();
	return _map[info.scale][info.areaIndex()].ways();
}

// simple linear search
id_t GuiRepresentation::closestPoint(const double lat, const double lon, const Bounds& bounds) const {
	id_t res = 0;
	auto temp = Node(0, lat, lon, {});

	double minDist = std::numeric_limits<double>().max();
	const auto info = bounds.scaleAndCoords();
	for (const id_t i : _map[info.scale][info.areaIndex()].nodes())
		if (Node::distance(temp, MapData::instance().nodes()[i]) < minDist) {
			minDist = Node::distance(temp, MapData::instance().nodes()[i]);
			res = i;
		}
	return res;
}