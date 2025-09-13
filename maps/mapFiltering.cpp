#include "mapFiltering.h"
#include <set>

MapInfoTuple dropUntraversableNodes(const Nodes& nodes,
									const Ways& ways,
									const std::vector<Relation>& relations) {
	Nodes resultNodes;
	Ways resultWays;
	std::vector<Relation> resultRelations;

	std::set<id_t> inclNodes;
	std::set<id_t> inclWays;

	for (const auto& way : ways) {
		if (way.refs().empty() || !way.hasTag("highway"))
			continue;
		if (way.hasTag("area") && way.tagValue("area") == "yes")
			continue;

		resultWays.emplace_back(way);
		inclWays.insert(way.id());
		for (const id_t id : way.refs())
			inclNodes.insert(id);
	}

	for (const auto& node : nodes)
		if (inclNodes.contains(node.id()))
			resultNodes.emplace_back(node);

	for (const auto& rel : relations) {
		const auto [from, via, to] = rel.refs();
		if (inclWays.contains(from) && inclWays.contains(to) && inclNodes.contains(via))
			resultRelations.emplace_back(rel);
	}

	return { resultNodes, resultWays, resultRelations };
}