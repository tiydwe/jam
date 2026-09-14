#include "Intersection.h"

#include "utility.h"
#include "Simulation.h"
#include "TrafficLight.h"

#include <algorithm>

#include "Layout.h"

Intersection::Intersection(Layout* parent)
  : _parent(parent),
    _id(utility::uid()),
    _trafficLight(std::make_unique<TrafficLight>(TrafficLight(_parent))) {}

Lights Intersection::getLightByLaneID(size_t laneid) const {
  // for now, always green
  return Lights::GREEN;
}

void Intersection::addOutgoing(size_t road_id) {
  _outgoings.push_back(road_id);
  _roads.push_back({road_id, EdgeType::OUTGOING});
  _trafficLight->addOutgoing(road_id);
}

void Intersection::removeOutgoing(size_t roadid) {
  auto it = std::find(_outgoings.begin(), _outgoings.end(), roadid);
  if(it == _outgoings.end()){
    utility::logWarn("Intersection::removeOutgoing - tried to remove road id " + std::to_string(roadid) + " but was not found.");
  }
  else{
    _outgoings.erase(it);
  }
  _roads.erase(std::remove(_roads.begin(), _roads.end(), std::make_pair(roadid, EdgeType::OUTGOING)), _roads.end());
  _trafficLight->removeRoad(roadid);
}

void Intersection::addIngoing(size_t roadid) {
  _roads.push_back({roadid, EdgeType::INGOING});
  _trafficLight->addIngoing(roadid);
}

void Intersection::removeIngoing(size_t roadid) {
  _roads.erase(std::remove(_roads.begin(), _roads.end(), std::make_pair(roadid, EdgeType::INGOING)), _roads.end());
  _trafficLight->removeRoad(roadid);
}

std::vector<std::pair<Road*, EdgeType>> Intersection::getRoads() const {
  std::vector<std::pair<Road*, EdgeType>> res;
  for(const auto& x : _roads){
    res.push_back({_parent->getRoad(x.first), x.second});
  }
  return res;
}
