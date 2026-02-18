#pragma once

#include "relation.h"
#include "graphRepresentation.h"
#include "guiRepresentation.h"

// class for the backend map representation.
class Map {
public:
	Map(const Nodes&, const Ways&, const std::vector<Relation>&, const Bounds&);

	// ways from the grid position according to current image boundaries
	const std::vector<id_t>& waysToVisualize(const Bounds&) const;
	// Dijkstra's algorithm
	std::vector<id_t> shortestPath(id_t from, id_t to);
	// closest map entry for the given arbitrary position
	id_t closestPoint(double lat, double lon, const Bounds&) const;

	inline const Node& node(id_t i) const { return _nodes[i]; }
	inline const Way& way(id_t i) const { return _ways[i]; }

private:
	// vector index corresponds to the Node / Way / Relation index.
	const Nodes _nodes;
	const Ways _ways;
	const std::vector<Relation> _relations;

	GuiRepresentation _guiRepresentation;
	GraphRepresentation _graphRepresentation;
};