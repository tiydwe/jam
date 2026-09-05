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
      _margin(margin) {
        _startLocation = location{initRoad, initLane, initDist};
      }

void Car::setDestination(const std::pair<size_t, double>& dest) {
  _current_destination = dest;
}

void Car::setRoute(const std::deque<size_t>& route) { _route = route; }

void Car::recalcRoute() {
  _route = _parentSim->findRoute(_position_roadid, _current_destination.first);
}

double Car::getCurrDistFrac() const {
  return _position_distance / _parentSim->getLane(_position_laneid).getLength();
}

void Car::_clipVelocity() {
  double speedLimit = _parentSim->getRoad(_position_roadid).getSpeedLimit();
  _velocity = std::max(0.0, _velocity);
  _velocity = std::min(_velocity, speedLimit);
}

double Car::_applyVelocity(double dt) {
  _position_distance += _velocity * dt;
  _parentSim->getLane(_position_laneid).moveCar(_position_distance, _id);
  return _velocity * dt;
}

std::string carStatusToString(carStatus s) {
  switch (s) {
    case carStatus::WAITING_AT_INTERSECTION:
      return "WAITING_AT_INTERSECTION";
    case carStatus::WAITING_FOR_NEXT_CAR:
      return "WAITING_FOR_NEXT_CAR";
    case carStatus::TRAVELING:
      return "TRAVELING";
    case carStatus::ARRIVING:
      return "ARRIVING";
    case carStatus::ARRIVED:
      return "ARRIVED";
    case carStatus::NO_ROUTE:
      return "NO_ROUTE";
    case carStatus::MERGING:
      return "MERGING";

    default:
      return "not recognized, " + std::to_string(static_cast<int>(s));
  }
}
