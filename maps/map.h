#pragma once

#include <vector>
#include <tuple>
#include <map>
#include <set>

#include "node.h"
#include "way.h"
#include "relation.h"
#include "bounds.h"
#include "connection.h"

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

	// map representation, optimized for GUI.
	class GuiRepresentation {
	public:
		GuiRepresentation() = delete;
		GuiRepresentation(const Nodes&, const Ways&, const Bounds&);

		// ways from the grid position according to current image boundaries
		const std::vector<id_t>& waysToVisualize(const Bounds&) const;
		// closest map entry for the given arbitrary position
		id_t closestPoint(double lat, double lon, const Nodes&, const Bounds&) const;

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
		// boundaries of the whole map.
		Bounds _fullBounds;
	};

	// map representation, optimized for the route search.
	class GraphRepresentation {
	public:
		GraphRepresentation() = delete;
		GraphRepresentation(const Nodes&, const Ways&);

		// Dijkstra's algorithm
		std::vector<id_t> shortestPath(id_t from, id_t to, const Nodes&);
	private:
		struct Connections {
			std::vector<Connection> input;
			std::vector<Connection> output;
		};

		// better name? Eliminates the "chain" connections.
		// remove traversable graph vertices that only have two neighboring vertices.
		void mergeNodes();
		// remove a traversable graph vertex, if it has two neighboring vertices. Returns the indices
		// of those neighboring vertices, if the vertex has been removed.
		std::set<id_t> mergeNode(id_t);
		// remove this and all neighboring vertices, if they only have two neighboring vertices.
		void mergeNodes(id_t);
		// re-introduce the vertex and the corresponding to it vertex "chain".
		void unfoldNodes(id_t, const Nodes&);

		// vector index corresponds to the referenced node, i.e. for each node, input and output connections
		// are to be found in Connections.
		std::vector<Connections> _connections;

		// the chains are merged. For the merged nodes, this saves the endpoints of a chain.
		// vector index = node id.
		std::vector<std::set<id_t>> _connectionRef;
	};

	GuiRepresentation _guiRepresentation;
	GraphRepresentation _graphRepresentation;
};