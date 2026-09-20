#pragma once

#include <vector>
#include <deque>
#include <random>

#include "Lane.h"

class Layout;
class RoadPhysical;
class Intersection;

// really shouldn't be having too many lanes, maybe max 4
class Road{
  public:

  Road();
  Road(Layout* parent, double speedLimit, size_t endIntersection, double angle);

  // adds lane always adds to edge
  void addLane(const size_t& lane){_lanes.push_back(lane);}

  // returns -2 if err
  // returns -1 if already at lane
  // returns adjacent lane in correct direction of target lane
  int laneToGoTo(size_t sourceLane, size_t targetLane);

  // returns -2 if err
  int closestLaneTo(size_t sourceLane, const std::vector<size_t> &targetLanes);

  // 0 equals center lane
  int getLanePosFromCenter(size_t laneid) const;

  size_t getID()const{return _id;}
  size_t getSpeedLimit()const{return _speed_limit;}
  const std::deque<size_t>& getLanes()const{return _lanes;}
  size_t getEdgeLane()const{return _lanes.front();}
  template <class Generator>
  size_t getRandomLane(Generator& g) const;
  size_t getEndIntersection()const{return _endIntersection;}
  Intersection* getTrueEndIntersection() const;
  size_t getNumLanes()const{return _lanes.size();}
  double getAngle()const{return _angle;}
  Lane* getLaneByID(size_t laneid) const;
  RoadPhysical* getRoadPhysical()const;

  // someday, refactor this so this stupid stuff doesn't need to be used
  Layout* getLayout() const {return _parent;}

  private:
  Layout* _parent;

  size_t _id;

  double _speed_limit;

  std::deque<size_t> _lanes;

  size_t _endIntersection;

  double _angle;
};

template <class Generator>
inline size_t Road::getRandomLane(Generator& g) const {
  return _lanes[std::uniform_int_distribution<>(0, _lanes.size()-1)(g)];
}
