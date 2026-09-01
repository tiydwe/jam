#pragma once

#include "Road.h"
#include "Lane.h"

#include <set>
#include <map>

#define MAX_LANES 30

class Simulation;

enum class Lights{
  RED, YELLOW, GREEN, INVALID
};

class RoadWrapper{
  public:
  RoadWrapper(Simulation& s, size_t roadid);
  bool operator<(const RoadWrapper &r) const;
  size_t getRoadID() const {return _roadid;}

  private:
  Simulation* _parent;
  size_t _roadid;
};

// SCHEDULE GUARENTEED SO THAT ALL ROADS CAN GO TO ALL OTHER ROADS
// schedule max 20 items for performance
// max lanes 30 for performance (also whose using that many lanes)

struct ScheduleItem{
  // valid[(i,j)]: whether a car can go from lane i to j
  // using map because laneids are not contigous
  std::map<std::pair<size_t, size_t>, Lights> valid;
  double duration;
};

// Handles an entire intersection lights
class TrafficLight{
  public:
  TrafficLight(Simulation& s);

  bool addIngoing(size_t roadid);
  bool addOutgoing(size_t roadid);

  void removeRoad(size_t roadid);

  // convert time to a schedule index
  size_t timeToScheduleItem(double time);

  std::vector<size_t> validTurns(size_t laneid, double time);
  
  // given a target road, what lanes could I be in to get to that road?
  std::vector<size_t> whereToTurn(size_t roadid);

  Lights getLightToLane(size_t laneidSource, size_t laneidTarget, double time);

  // returns a lane id ON THE NEW ROAD if can turn onto a lane on road, otherwise -1
  // if multiple possible lanes, will choose arbitrarly
  int getLaneCanTurnOnRoad(size_t laneidSource, size_t roadidTarget, double time);

  // dumb scheduler, just gives each road a turn to go every which way and splits it evenly
  void reSchedule(double greenPhaseTime, double yellowPhaseTime);

  // makes sure that all paths at all times are have a light. will set paths as either RED or INVALID if not already filled
  void normalizeSchedule();

  private:
  Simulation* _parent;

  // IN MEANS INTO THE INTERSECTION
  std::set<RoadWrapper> _ingoingRoads;
  std::set<RoadWrapper> _outgoingRoads;

  size_t _totalLanes;
  
  std::vector<ScheduleItem> _schedule;
};