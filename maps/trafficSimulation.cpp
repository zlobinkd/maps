#include "trafficSimulation.h"

#include "trafficCar.h"
#include "trafficLight.h"
#include "mapData.h"

#include <algorithm>

void TrafficSimulation::run() {
	for (size_t i = 0; i < 10000000; i++)
		updateStep();

	dump();
}

void TrafficSimulation::dump() const
{}

void TrafficSimulation::updateStep() {
	fillDummies();
	updateObjects();
	clearDummies();
	deleteOffMapObjects();
	addNewObjects();
}

std::optional<std::pair<TrafficDummy, double>> TrafficSimulation::findNextObject(const TrafficObject& object) const
{
	const auto route = object.route();
	if (route.empty())
		return std::nullopt;

	double distance = 0.;
	for (size_t i = 0; i < route.size(); i++)
	{
		for (auto& iter : _dummies[route[i].from()]) {
			if (iter.first.to() == route[i].to() && iter.first.way().id() == object.currentSegment().way().id()) {
				if (i == 0) {
					const auto& it = std::find_if(iter.second.begin(), iter.second.end(),
						[&object](const TrafficDummy& d) {return d.progress() > object.progressOnCurrentSegment(); });

					if (it != iter.second.end())
						return std::make_pair(*it, 
							(it->progress() - object.progressOnCurrentSegment()) * object.currentSegment().distance());

					distance += (1. - object.progressOnCurrentSegment()) * object.currentSegment().distance();
				}
				else {
					if (!iter.second.empty())
						return std::make_pair(iter.second.front(), 
							distance + iter.second.front().progress() * iter.first.distance());

					distance += iter.first.distance();
				}
			}
		}
	}
	return std::nullopt;
}

void TrafficSimulation::fillDummies() {
	for (const auto& object : _objects)
		if (object.isObstacle() && object.isOnMap())
			for (auto& [connection, dummies] : _dummies[object.currentSegment().from()])
				if (connection.way().id() == object.currentSegment().way().id() && connection.to() == object.currentSegment().to())
					dummies.emplace_back(object);

	for (auto& iter : _dummies)
		for (auto& [connection, dummies] : iter)
			std::sort(dummies.begin(), dummies.end(),
				[](const TrafficDummy& d1, const TrafficDummy& d2) { return d1.progress() < d2.progress(); });
}

void TrafficSimulation::updateObjects() {
	for (auto& object : _objects)
	{
		const auto nextObjInfo = findNextObject(object);
		if (!nextObjInfo.has_value())
			object.update(1e7, 100.);

		object.update(nextObjInfo->second, nextObjInfo->first.speed());
	}
}

void TrafficSimulation::clearDummies() {
	for (auto& iter : _dummies)
		for (auto& [_, dummies] : iter)
			dummies.clear();
}
void TrafficSimulation::addNewObjects() {}

void TrafficSimulation::deleteOffMapObjects() {
	std::vector<size_t> idsToDelete;

	for (size_t i = 0; i < _objects.size(); i++)
		if (!_objects[i].isOnMap())
			idsToDelete.push_back(i);

	std::reverse(idsToDelete.begin(), idsToDelete.end());

	for (size_t idToDelete : idsToDelete)
		_objects.erase(_objects.begin() + idToDelete, _objects.begin() + idToDelete + 1);
}