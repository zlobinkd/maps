#include "map.h"

#include <algorithm>
#include <iostream>

Map::Map(const Nodes& nodes,
		 const Ways& ways,
		 const std::vector<Relation>& relations,
		 const Bounds& bounds) :
	_nodes(nodes),
	_ways(ways),
	_relations(relations),
	_guiRepresentation(_nodes, _ways, bounds),
	_graphRepresentation(_nodes, _ways)
{
}

const std::vector<id_t>& Map::waysToVisualize(const Bounds& bounds) const {
	return _guiRepresentation.waysToVisualize(bounds);
}

std::vector<id_t> Map::shortestPath(id_t from, id_t to) {
	return _graphRepresentation.shortestPath(from, to, _nodes);
}

id_t Map::closestPoint(double lat, double lon, const Bounds& bounds) const {
	return _guiRepresentation.closestPoint(lat, lon, _nodes, bounds);
}