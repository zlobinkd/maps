#pragma once

#include "core.h"

#include "node.h"
#include "way.h"
#include "relation.h"
#include "bounds.h"

class MapData {
public:
	static const MapData& instance()
	{
		static MapData instance;
		return instance;
	}

	MapData(const MapData&) = delete;
	MapData& operator=(const MapData&) = delete;
	MapData(MapData&&) = delete;
	MapData& operator=(MapData&&) = delete;

	const Nodes& nodes() const { return _nodes; }
	const Ways& ways() const { return _ways; }
	const std::vector<Relation>& relations() const { return _relations; }
	const Bounds& bounds() const { return _bounds; }

private:
	MapData();

	Nodes _nodes;
	Ways _ways;
	std::vector<Relation> _relations;
	Bounds _bounds;
};