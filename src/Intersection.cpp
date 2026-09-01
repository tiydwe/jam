#include "Intersection.h"

#include "utility.h"
#include "Simulation.h"
#include "TrafficLight.h"

#include <algorithm>


Intersection::Intersection(Simulation& parent)
  : _parent(&parent),
    _id(utility::uid()),
    _trafficLight(TrafficLight(*_parent)) {}

Lights Intersection::getLightByLaneID(size_t laneid) const {
  // for now, always green
  return Lights::GREEN;
}

void Intersection::addOutgoing(size_t road_id) {
  _outgoings.push_back(road_id);
  _trafficLight.addOutgoing(road_id);
}

void Intersection::removeOutgoing(size_t roadid) {
  auto it = std::find(_outgoings.begin(), _outgoings.end(), roadid);
  if(it == _outgoings.end()){
    utility::logWarn("Intersection::removeOutgoing - tried to remove road id " + std::to_string(roadid) + " but was not found.");
  }
  else{
    _outgoings.erase(it);
  }
  _trafficLight.removeRoad(roadid);
}

void Intersection::addIngoing(size_t roadid) {
  _trafficLight.addIngoing(roadid);
}

void Intersection::removeIngoing(size_t roadid) {
  _trafficLight.removeRoad(roadid);
}
