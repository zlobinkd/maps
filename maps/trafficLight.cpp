#include "trafficLight.h"

TrafficLight::TrafficLight(Connection segment, size_t counterStartPhase, size_t redTime, size_t greenTime) 
	: _segment(segment), _timeCounter(counterStartPhase), _redTime(redTime), _greenTime(greenTime)
{}

void TrafficLight::update(double, double) 
{
	_timeCounter++;
	if (_timeCounter >= _redTime + _greenTime)
		_timeCounter = 0;
}

double TrafficLight::speed() const {
	return 0.;
}

double TrafficLight::progressOnCurrentSegment() const {
	return 1.;
}
Connection TrafficLight::currentSegment() const {
	return _segment;
}

bool TrafficLight::isObstacle() const {
	return _timeCounter < _redTime;
}