#pragma once

#include <vector>
#include <deque>

class Simulation;

enum class carStatus {
  WAITING_AT_INTERSECTION,
  WAITING_FOR_NEXT_CAR,
  TRAVELING
};

struct scheduleItem {
  std::pair<size_t, size_t> dest;
  size_t targetArrival;
  size_t earliestLeaving;
};

class Car {
 public:
  Car();
  Car(Simulation& parent, size_t initRoad, size_t initLane, size_t initDist, double max_a, double margin=1.0);
  void setDestination(const std::pair<size_t, size_t>& dest);

  void move(double dt);

  void setRoute(const std::deque<size_t> &route);

  size_t getID() const { return _id; }

 private:
  void _clipVelocity();
  void _decelerate(double a, double dt) { _velocity -= a * dt; _clipVelocity();}
  void _accelerate(double a, double dt) { _velocity += a * dt; _clipVelocity();}
  void _applyVelocity(double dt){_position_distance += _velocity * dt;}

  Simulation* _parentSim;

  size_t _id;
  carStatus _status;

  size_t _position_roadid;
  size_t _position_laneid;
  double _position_distance;

  // roadid (not laneid)
  std::deque<size_t> _route;

  // roadid, distance
  std::pair<size_t, size_t> _current_destination;

  // std::vector<scheduleItem>

  double _max_acceleration;
  double _velocity;
  double _minStoppingDist;
  double _margin;
};