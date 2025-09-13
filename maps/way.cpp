#include "way.h"
#include "bounds.h"

#include <ranges>

Way::Way(id_t id, const std::vector<id_t>& nodeIds, const Tags& tags)
	: _id(id), _nodeIds(nodeIds), _tags(tags) 
{}

id_t Way::id() const {
	return _id;
}

void Way::setId(id_t id) {
	_id = id;
}

bool Way::hasTag(const std::string& tag) const {
	for (const auto& [key, _] : _tags)
		if (key == tag)
			return true;

	return false;
}

const std::string& Way::tagValue(const std::string& tag) const {
	for (const auto& [key, value] : _tags)
		if (key == tag)
			return value;

	return "";
}

const std::vector<id_t>& Way::refs() const {
	return _nodeIds;
}