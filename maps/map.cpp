#include "map.h"

#include <algorithm>
#include <iostream>

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

void Map::GuiRepresentation::Area::insertNode(const id_t id) {
	_nodes.push_back(id);
}

void Map::GuiRepresentation::Area::insertWay(const id_t id) {
	_ways.push_back(id);
}

const std::vector<id_t>& Map::GuiRepresentation::Area::nodes() const {
	return _nodes;
}

const std::vector<id_t>& Map::GuiRepresentation::Area::ways() const {
	return _ways;
}

Map::GuiRepresentation::GuiRepresentation(const Nodes& nodes, 
										  const Ways& ways, 
										  const Bounds& bounds) 
	: _fullBounds(bounds) 
{
	// init grid
	for (size_t scale = 0; scale < SCALES_NUM; scale++) {
		_map.push_back({});
		const size_t areasNum2 = 1 << (scale << 1);
		for (size_t i = 0; i < areasNum2; i++)
			_map.back().push_back(Area{});
	}

	// assign nodes to the grid positions
	for (const Node& node : nodes) {
		const auto scalesAndCoords = _fullBounds.scalesAndCoords(node);
		for (size_t scale = 0; scale < scalesAndCoords.size(); scale++) {
			const auto& areaIndX = scalesAndCoords[scale][0];
			const auto& areaIndY = scalesAndCoords[scale][1];
			const size_t areasNum = 1 << scale;
			for (size_t i = areaIndX[0]; i <= areaIndX[1]; i++)
				for (size_t j = areaIndY[0]; j <= areaIndY[1]; j++)
					_map[scale][i * areasNum + j].insertNode(node.id());
		}
	}

	// assign ways to the grid positions: secondary roads et al. only appear when we zoom in.
	for (const Way& way : ways) {
		const auto scalesAndCoords = _fullBounds.scalesAndCoords(way, nodes);
		for (size_t scale = 0; scale < scalesAndCoords.size(); scale++) {
			if (!way.hasTag("highway") || scale < 2 && !isMainRoad(way.tagValue("highway")))
				continue;

			if (scale < 4 && !isRoad(way.tagValue("highway")))
				continue;

			const auto& areaIndX = scalesAndCoords[scale][0];
			const auto& areaIndY = scalesAndCoords[scale][1];
			const size_t areasNum = 1 << scale;
			for (size_t i = areaIndX[0]; i <= areaIndX[1]; i++)
				for (size_t j = areaIndY[0]; j <= areaIndY[1]; j++)
					_map[scale][i * areasNum + j].insertWay(way.id());
		}
	}
}

const std::vector<id_t>& Map::GuiRepresentation::waysToVisualize(const Bounds& bounds) const {
	const auto [scale, x, y] = _fullBounds.scaleAndCoords(bounds);
	return _map[scale][(1 << scale) * x + y].ways();
}

// simple linear search
id_t Map::GuiRepresentation::closestPoint(double lat, double lon, const Nodes& nodes, const Bounds& bounds) const {
	id_t res = 0;
	auto temp = Node(0, lat, lon, {});

	double minDist = std::numeric_limits<double>().max();
	const auto [scale, x, y] = _fullBounds.scaleAndCoords(bounds);
	for (const id_t i : _map[scale][(1 << scale) * x + y].nodes())
		if (Node::distance(temp, nodes[i]) < minDist) {
			minDist = Node::distance(temp, nodes[i]);
			res = i;
		}
	return res;
}

Map::GraphRepresentation::GraphRepresentation(const Nodes& nodes, const Ways& ways) 
{
	_connections = std::vector<Connections>(nodes.size(), {});
	_connectionRef = std::vector<std::set<id_t>>(nodes.size(), {});

	for (const auto& way : ways) {
		for (size_t i = 1; i < way.refs().size(); i++) {
			const id_t node1 = way.refs()[i - 1];
			const id_t node2 = way.refs()[i];
			_connections[node1].output.emplace_back(Connection{ &way, node1, node2, nodes });
			_connections[node2].input.emplace_back(Connection{ &way, node1, node2, nodes });
		}

		//if both ways not allowed
		if (way.tagValue("oneway") == "yes")
			continue;

		for (size_t i = 1; i < way.refs().size(); i++) {
			const id_t node1 = way.refs()[i - 1];
			const id_t node2 = way.refs()[i];
			_connections[node2].output.emplace_back(Connection{ &way, node2, node1, nodes });
			_connections[node1].input.emplace_back(Connection{ &way, node2, node1, nodes });
		}
	}

	// under construction
	mergeNodes();
}

void Map::GraphRepresentation::mergeNodes() {
	for (size_t i = 0; i < _connections.size(); i++)
		mergeNode(i);
}

std::set<id_t> Map::GraphRepresentation::mergeNode(id_t i) {
	// not equal output and input: cannot remove node
	if (_connections[i].input.size() != _connections[i].output.size())
		return {};

	// no connections or more than two neighboring nodes: cannot remove
	if (_connections[i].input.size() == 0 || _connections[i].input.size() > 2)
		return {};

	// ways not the same: cannot remove (for safety)
	const auto* way = &_connections[i].input.front().way();
	if (&_connections[i].input.back().way() != way)
		return {};

	if (&_connections[i].output.front().way() != way)
		return {};

	if (&_connections[i].output.back().way() != way)
		return {};

	// align inputs and outputs
	// from  1->2, 2->1  make  1->2, 2->3
	//       3->2, 2->3        3->2, 2->1
	if (_connections[i].input.front().from() == _connections[i].output.front().to())
	{
		if (_connections[i].input.size() == 1)
			return {};
		std::reverse(_connections[i].output.begin(), _connections[i].output.end());
	}

	for (size_t j = 0; j < _connections[i].input.size(); j++) {
		id_t from = _connections[i].input[j].from();
		id_t to = _connections[i].output[j].to();

		// remove old connections.
		for (int k = _connections[from].output.size() - 1; k >= 0; k--)
			if (_connections[from].output[k].to() == i && &_connections[from].output[k].way() == way)
				_connections[from].output.erase(_connections[from].output.begin() + k);

		for (int k = _connections[to].input.size() - 1; k >= 0; k--)
			if (_connections[to].input[k].from() == i && &_connections[to].input[k].way() == way)
				_connections[to].input.erase(_connections[to].input.begin() + k);

		// add new connections.
		auto newConnection = Connection{ _connections[i].input[j], _connections[i].output[j] };
		_connections[from].output.push_back(newConnection);
		_connections[to].input.push_back(newConnection);

		// set new node references.
		for (id_t k : newConnection.path()) {
			_connectionRef[k].clear();
			_connectionRef[k].insert(from);
			_connectionRef[k].insert(to);
		}
	}

	// remove node... goodbye...
	_connections[i].input.clear();
	_connections[i].output.clear();

	return _connectionRef[i];
}

void Map::GraphRepresentation::mergeNodes(id_t i) {
	std::set<id_t> ids = { i };
	while (!ids.empty()) {
		std::set<id_t> newIds;
		for (id_t j : ids)
			for (id_t k : mergeNode(j))
				newIds.insert(k);
		ids = newIds;
	}
}

void Map::GraphRepresentation::unfoldNodes(id_t i, const Nodes& nodes) {
	// there must be the left and right referenced nodes
	if (_connectionRef[i].size() != 2)
		return;

	const id_t left = *_connectionRef[i].begin();
	const id_t right = *(_connectionRef[i].rbegin());

	for (const auto& connection : _connections[left].input) {
		if (connection.from() != right)
			continue;

		const auto& path = connection.path();
		if (std::find(path.begin(), path.end(), i) == path.end())
			continue;

		for (const auto& newConnection : connection.explode(nodes))
		{
			_connections[newConnection.to()].input.push_back(newConnection);
			_connections[newConnection.from()].output.push_back(newConnection);
			_connectionRef[newConnection.to()].clear();
		}
	}

	for (const auto& connection : _connections[left].output) {
		if (connection.to() != right)
			continue;

		const auto& path = connection.path();
		if (std::find(path.begin(), path.end(), i) == path.end())
			continue;

		for (const auto& newConnection : connection.explode(nodes))
		{
			_connections[newConnection.from()].output.push_back(newConnection);
			_connections[newConnection.to()].input.push_back(newConnection);
			_connectionRef[newConnection.from()].clear();
		}
	}

	if (_connectionRef[i].empty()) {
		for (int j = _connections[left].input.size() - 1; j >= 0; j--)
		{
			const auto& path = _connections[left].input[j].path();
			if (_connections[left].input[j].from() == right && std::find(path.begin(), path.end(), i) != path.end())
				_connections[left].input.erase(_connections[left].input.begin() + j);
		}

		for (int j = _connections[right].input.size() - 1; j >= 0; j--)
		{
			const auto& path = _connections[right].input[j].path();
			if (_connections[right].input[j].from() == left && std::find(path.begin(), path.end(), i) != path.end())
				_connections[right].input.erase(_connections[right].input.begin() + j);
		}

		for (int j = _connections[left].output.size() - 1; j >= 0; j--)
		{
			const auto& path = _connections[left].output[j].path();
			if (_connections[left].output[j].to() == right && std::find(path.begin(), path.end(), i) != path.end())
				_connections[left].output.erase(_connections[left].output.begin() + j);
		}

		for (int j = _connections[right].output.size() - 1; j >= 0; j--)
		{
			const auto& path = _connections[right].output[j].path();
			if (_connections[right].output[j].to() == left && std::find(path.begin(), path.end(), i) != path.end())
				_connections[right].output.erase(_connections[right].output.begin() + j);
		}
	}
}

std::vector<id_t> Map::GraphRepresentation::shortestPath(id_t from, id_t to, const Nodes& nodes) {
	unfoldNodes(from, nodes);
	unfoldNodes(to, nodes);
	// init algorithm containers
	auto routeLengths = std::vector<double>(nodes.size(), std::numeric_limits<double>().max());
	auto prevNodes = std::vector<std::pair<id_t, const Way*>>(nodes.size(), { 0, nullptr });
	routeLengths[from] = 0.;
	prevNodes[from] = { from, nullptr };
	auto currentNodes = std::set<id_t>{from};

	// Dijkstra
	while (!currentNodes.empty()) {
		auto nextNodes = std::set<id_t>{};
		for (const id_t node : currentNodes) {
			for (const auto& connection : _connections[node].output) {
				const double maxSpeed = connection.way().speedLimit();
				if (routeLengths[connection.to()] > routeLengths[connection.from()] + connection.distance() / maxSpeed) {
					routeLengths[connection.to()] = routeLengths[connection.from()] + connection.distance() / maxSpeed;
					prevNodes[connection.to()] = { connection.from(), &connection.way()};
					if (routeLengths[connection.to()] < routeLengths[to])
					{
						nextNodes.insert(connection.to());
					}
				}
			}
		}
		currentNodes = nextNodes;
	}

	// find the actual route
	auto route = std::vector<id_t>{to};
	id_t currentNode = to;
	while (currentNode != from)
	{
		for (const auto& connection : _connections[currentNode].input) {
			if (connection.from() == prevNodes[currentNode].first && &connection.way() == prevNodes[currentNode].second) {
				route.insert(route.begin(), connection.path().begin(), connection.path().end());
				break;
			}
		}
		currentNode = prevNodes[currentNode].first;
		route.insert(route.begin(), currentNode);
	}

	mergeNodes(from);
	mergeNodes(to);

	return route;
}

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