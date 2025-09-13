#pragma once

#include <optional>
#include <vector>

#include "core.h"
#include "relation.h"

class Way;
class Node;

// Class defining a graph edge.
class Connection {
public:
	Connection() = delete;
	Connection(const Way* way, id_t from, id_t to, const std::vector<Node>&);
	Connection(const Way* way, id_t from, id_t to, const std::vector<id_t>& path, const std::vector<Node>&);
	// TODO: this must not always be successful. left.to() must be equal to right.from()
	Connection(const Connection& left, const Connection& right);

	inline id_t to() const { return _to; }
	inline id_t from() const { return _from; }
	// referenced way
	inline const Way& way() const { return *_way; }
	// points between <from> and <to>
	inline const std::vector<id_t>& path() const { return _path; }
	// distance from <from> to <to>, anong the <path>
	inline double distance() const { return _distance; }

private:
	const Way* _way;
	id_t _from;
	id_t _to;
	std::vector<id_t> _path;

	// cached for performance
	double _distance = 0.;
};