#pragma once

#include "core.h"

#include "trafficObject.h"

class TrafficLight : public TrafficObject {
public:
	TrafficLight(Connection segment, size_t counterStartPhase, size_t redTime, size_t greenTime);
	~TrafficLight() override = default;

	void update(double distanceToNextObject, double nextObjectSpeed) override;
	double speed() const override;
	double progressOnCurrentSegment() const override;
	Connection currentSegment() const override;

	virtual bool isObstacle() const override;

private:
	const Connection _segment;
	size_t _timeCounter;
	const size_t _redTime;
	const size_t _greenTime;
};