#include "connection.h"
#include "way.h"
#include "node.h"

Connection::Connection(const Way* way, id_t from, id_t to, const Nodes& nodes) 
	: _way(way), _from(from), _to(to), _path({}), _distance(Node::distance(nodes[from], nodes[to]))
{}

Connection::Connection(const Way* way, id_t from, id_t to, const std::vector<id_t>& path, const Nodes& nodes)
	: _way(way), _from(from), _to(to), _path(path)
{
	if (path.empty())
		_distance += Node::distance(nodes[from], nodes[to]);
	else {
		_distance += Node::distance(nodes[from], nodes[path.front()]);
		_distance += Node::distance(nodes[to], nodes[path.back()]);
		for (size_t i = 1; i < path.size(); i++)
			_distance += Node::distance(nodes[path[i - 1]], nodes[path[i]]);
	}
}

Connection::Connection(const Connection& left, const Connection& right)
	: _way(left._way), _from(left.from()), _to(right.to()), _distance(left.distance() + right.distance()) 
{
	_path = left._path;
	_path.push_back(left.to()); // == right.from()
	_path.insert(_path.end(), right._path.begin(), right._path.end());
}