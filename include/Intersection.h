#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "TrafficLight.h"

class Layout;

enum class EdgeType { INGOING, OUTGOING };

class Intersection {
 public:
  Intersection(Layout* parent);

  size_t getID() const { return _id; }

  Lights getLightByLaneID(size_t laneid) const;

  const std::vector<size_t>& getOutgoings() const { return _outgoings; }
  void addOutgoing(size_t road_id);
  void removeOutgoing(size_t roadid);

  void addIngoing(size_t roadid);
  void removeIngoing(size_t roadid);

  TrafficLight* getTrafficLight() { return _trafficLight.get(); }
  void setTrafficLight(std::unique_ptr<TrafficLight> tl) {_trafficLight = std::move(tl);};

 private:
  Layout* _parent;

  std::unique_ptr<TrafficLight> _trafficLight;

  size_t _id;

  // road ids
  std::vector<size_t> _outgoings;
  // clockwise
  std::vector<std::pair<size_t, EdgeType>> _roads;

  // TODO: schedule
  // for now, always green
  std::vector<ScheduleItem> _schedule;
};
