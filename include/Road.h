#pragma once

#include <vector>

class Simulation;


class Road{
  public:

  Road();
  Road(Simulation& parent, double speedLimit, size_t endIntersection);

  void addLane(const size_t& lane){_lanes.push_back(lane);}
  void setEdgeLane(const size_t& lane){_edgeLane = lane;}

  size_t getID()const{return _id;}
  size_t getSpeedLimit()const{return _speed_limit;}
  size_t getEdgeLane()const{return _edgeLane;}
  size_t getEndIntersection()const{return _endIntersection;}

  private:
  Simulation* _parent;

  size_t _id;

  double _speed_limit;

  std::vector<size_t> _lanes;

  size_t _edgeLane;

  size_t _endIntersection;
};