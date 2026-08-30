#include "Car.h"

#include "Simulation.h"
#include "utility.h"

Car::Car() {
  utility::exit(
      "This should never be called (probally something messed up in map!)");
}

Car::Car(Simulation& parent, size_t initRoad, size_t initLane, size_t initDist,
         double max_a, double margin)
    : _parentSim(&parent),
      _id(utility::uid()),
      _status(carStatus::TRAVELING),
      _position_roadid(initRoad),
      _position_laneid(initLane),
      _position_distance(initDist),
      _max_acceleration(max_a),
      _velocity(0.0),
      _minStoppingDist(0.0),
      _margin(margin) {}

void Car::setDestination(const std::pair<size_t, size_t>& dest) {
  _current_destination = dest;
}

void Car::move(double dt) {
  // if car is coming up on desination, slow down
  // TODO implement lane checking
  _minStoppingDist = _velocity*_velocity/2.0/_max_acceleration;
  bool didA = false;
  if (_position_roadid == _current_destination.first &&
      _current_destination.second - _position_distance <=
          _minStoppingDist + _margin) {

#ifdef DEBUG
  utility::log("Target spotted at  " + std::to_string(_position_distance));
#endif
    _decelerate(_max_acceleration, dt);
    didA = true;
  }
  // otherwise, accelerate slower
  if (!didA) {
    _accelerate(_max_acceleration * 0.8, dt);
  }
  _applyVelocity(dt);

#ifdef DEBUG
  utility::log("Moved to " + std::to_string(_position_distance));
#endif
}

void Car::_clipVelocity() {
  double speedLimit = _parentSim->getRoad(_position_roadid).getSpeedLimit();
  _velocity = std::max(0.0, _velocity);
  _velocity = std::min(_velocity, speedLimit);
}
