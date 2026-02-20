#pragma once

#include "core.h"

#include "trafficObject.h"

class TrafficCar : public TrafficObject {
public:
	TrafficCar(const std::vector<Connection>& route);
	~TrafficCar() override = default;

	void update(double distanceToNextObject, double nextObjectSpeed) override;
	double speed() const override;
	double progressOnCurrentSegment() const override;
	Connection currentSegment() const override;
	bool isOnMap() const override;

private:
	const std::vector<Connection> _route;
	double _speed = 0.;
	double _progressOnCurrentSegment = 0.;
	size_t _currentConnectionId = 0;
};