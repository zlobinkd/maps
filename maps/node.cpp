#include "node.h"
#include "bounds.h"
#include <cmath>

#define M_PI           3.14159265358979323846

Node::Node(id_t id, double lat, double lon, const Tags& tags)
	: _id(id), _lat(lat), _lon(lon), _tags(tags)
{}

id_t Node::id() const {
	return _id;
}

void Node::setId(id_t id) {
	_id = id;
}

std::array<double, 2> Node::localCoords(const Bounds& bounds) const {
	return bounds.localCoords(_lat, _lon);
}

double Node::distance(const Node& first, const Node& second) {
	const double theta1 = first._lat * M_PI / 180.;
	const double theta2 = second._lat * M_PI / 180.;
	const double phi1 = first._lon * M_PI / 180.;
	const double phi2 = second._lon * M_PI / 180.;

	const double dx2 = std::pow(phi1 - phi2, 2) * cos(theta1) * cos(theta2);
	const double dy2 = std::pow(theta1 - theta2, 2);

	return 6.4e6 * sqrt(dy2 + dx2);
}