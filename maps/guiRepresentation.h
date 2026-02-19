#pragma once

#include "node.h"
#include "way.h"
#include "bounds.h"

// map representation, optimized for GUI.
class GuiRepresentation {
public:
	GuiRepresentation();

	// ways from the grid position according to current image boundaries
	const std::vector<id_t>& waysToVisualize(const Bounds&) const;
	// closest map entry for the given arbitrary position
	id_t closestPoint(double lat, double lon, const Bounds&) const;

private:
	// grid position entry.
	class Area {
	public:
		Area() = default;

		void insertNode(id_t);
		void insertWay(id_t);

		const std::vector<id_t>& nodes() const;
		const std::vector<id_t>& ways() const;

	private:
		std::vector<id_t> _nodes;
		std::vector<id_t> _ways;
	};

	// grid.
	using MapImpl = std::vector<std::vector<Area>>;

	MapImpl _map;
};