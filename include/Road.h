#pragma once

#include <vector>
#include <deque>

class Simulation;


// really shouldn't be having too many lanes, maybe max 4
class Road{
  public:

  Road();
  Road(Simulation& parent, double speedLimit, size_t endIntersection, double angle);

  // adds lane always adds too center, not edge
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
  size_t getEdgeLane()const{return _lanes.back();}
  size_t getEndIntersection()const{return _endIntersection;}
  size_t getNumLanes()const{return _lanes.size();}
  double getAngle()const{return _angle;}


  private:
  Simulation* _parent;

  size_t _id;

  double _speed_limit;

  // front = left (center), back = right (edge)
  std::deque<size_t> _lanes;

  size_t _endIntersection;

  double _angle;
};