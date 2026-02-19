#include "mapData.h"
#include "myXmlParser.h"

MapData::MapData() {
	auto res = parseXML("path\\to\\file");

	if (!res.has_value())
		return;

	auto& [nodes, ways, relations, bounds] = *res;

	_nodes = nodes;
	_ways = ways;
	_relations = relations;
	_bounds = bounds;
}