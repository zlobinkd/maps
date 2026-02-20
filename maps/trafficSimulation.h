#pragma once

#include "core.h"

#include "trafficObject.h"
#include "trafficDummy.h"
#include "connection.h"

#include <vector>

class TrafficSimulation {
public:
	void run();
	void dump() const;
	
private:
	void updateStep();
	std::optional<std::pair<TrafficDummy, double>> findNextObject(const TrafficObject&);
	void fillDummies();
	void updateObjects();
	void clearDummies();
	void addNewObjects();
	void deleteOffMapObjects();

	using Connections = std::vector<std::pair<Connection, std::vector<TrafficDummy>>>;


	std::vector<Connections> _dummies;
	std::vector<TrafficObject> _objects;
};