#pragma once

#include "core.h"
#include "connection.h"

class TrafficObject {
public:
	virtual ~TrafficObject() = default;

	virtual void update(double distanceToNextObject, double nextObjectSpeed) = 0;
	virtual double speed() const = 0;
	virtual double progressOnCurrentSegment() const = 0;
	virtual Connection currentSegment() const = 0;
	virtual const std::vector<Connection> route() const;
	virtual bool isOnMap() const;

	virtual bool isObstacle() const;
};