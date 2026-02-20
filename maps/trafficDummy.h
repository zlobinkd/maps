#pragma once

#include "core.h"

class TrafficObject;

class TrafficDummy {
public:
	TrafficDummy(const TrafficDummy&) = default;
	TrafficDummy(const TrafficObject&);

	double speed() const { return _speed; }
	double progress() const { return _progress; }
private:
	double _speed;
	double _progress;
};