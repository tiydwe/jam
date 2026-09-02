#pragma once

#include <vector>
#include <deque>
#include <string>

class Simulation;

enum class carStatus {
  WAITING_AT_INTERSECTION,
  WAITING_FOR_NEXT_CAR,
  TRAVELING,
  ARRIVING,
  ARRIVED,
  NO_ROUTE,
  MERGING
};

std::string carStatusToString(carStatus s);

struct scheduleItem {
  std::pair<size_t, size_t> dest;
  size_t targetArrival;
  size_t earliestLeaving;
};

class Car {
 public:
  Car();
  Car(Simulation& parent, size_t initRoad, size_t initLane, size_t initDist, double max_a, double margin=1.0);
  void setDestination(const std::pair<size_t, double>& dest);

  void move(double dt);

  void setRoute(const std::deque<size_t> &route);

  size_t getID() const { return _id; }
  double getDistance() const {return _position_distance; }

  size_t getCurrRoad() const {return _position_roadid; }
  size_t getCurrLane() const {return _position_laneid; }
  double getCurrDist() const {return _position_distance; }
  double getCurrDistFrac() const;

 private:
  void _clipVelocity();
  void _decelerate(double a, double dt) { _velocity -= a * dt; _clipVelocity();}
  void _accelerate(double a, double dt) { _velocity += a * dt; _clipVelocity();}
  double _applyVelocity(double dt);

  // will automatically roll for chance
  bool _mergeToNextLane(double dd);

  void _init_merge(size_t newLaneid);
  // false = not merged, true = merged
  bool _check_merge();
  bool _move_checkIntersection(double dt);

  Simulation* _parentSim;

  size_t _id;
  carStatus _status;

  size_t _position_roadid;
  size_t _position_laneid;
  // back of car
  double _position_distance;

  // roadid (not laneid)
  std::deque<size_t> _route;

  // roadid, distance
  std::pair<size_t, double> _current_destination;

  // std::vector<scheduleItem>

  double _max_acceleration;
  double _velocity;
  double _minStoppingDist;

  // car length + safe space
  double _margin;

  // only valid during a merge
  size_t _newLaneMerge;
  double _newMergedDistance;
};