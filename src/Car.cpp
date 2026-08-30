#include "Car.h"

#include <algorithm>

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
  _minStoppingDist = _velocity * _velocity / 2.0 / _max_acceleration;
  bool didA = false;
  bool foundDestination = false;
  if (_position_roadid == _current_destination.first &&
      _current_destination.second - _position_distance <=
          _minStoppingDist + _margin) {
#ifdef DEBUG
    utility::log("Target spotted at  " + std::to_string(_position_distance));
#endif
    _decelerate(_max_acceleration, dt);
    didA = true;
    foundDestination = true;
  }

  // if car coming up at end of road, check intersection
  if (_position_distance + _minStoppingDist + _margin >
          _parentSim->getLane(_position_laneid).getLength() &&
      !foundDestination) {
    // coming up on next intersection
    // make sure that route has next road
    if (_route.size() == 0) {
#ifdef DEBUG
      utility::logErr(
          "While trying to Car::move, route ended without finding "
          "destination.");
      utility::exit();
#endif
      // behaviour: car will not move without valid route
      _decelerate(_max_acceleration, dt);
      didA = true;
    }
    // make sure that route is in correct format, does not include current road
    if (_route.front() == _position_roadid) {
      utility::logWarn("While trying to Car::move, route had current road");
      // fix
      _route.pop_front();
    } else {
      const auto road = _parentSim->getRoad(_route.front());
      _route.pop_front();
      const auto intersection =
          _parentSim->getIntersection(_parentSim->getRoad(_position_roadid).getEndIntersection());
      if (intersection.getLightByLaneID(_position_laneid) == Lights::GREEN) {
        // go to next road.
        if (std::find(intersection.getOutgoings().begin(),
                      intersection.getOutgoings().end(),
                      road.getID()) == intersection.getOutgoings().end()) {
          // intersection does not have road
#ifdef DEBUG
          utility::logErr(
              "While trying to Car::move, intersection did no have road on "
              "route");
          utility::logErr("Outgoings was " +
                          std::to_string(intersection.getOutgoings().size()));
          utility::exit();
#endif
          _decelerate(_max_acceleration, dt);
          didA = true;
        } else {
          // TODO: make it so car doesn't just teleport
          double newDist = 0.0;
          // TODO: implement intersection lane turning rules
          size_t newLane = road.getEdgeLane();
          size_t newRoadID = road.getID();
          // make sure there arn't any cars in the way
          if (_parentSim->getLane(newLane).minDistance().first <
              newDist + _minStoppingDist + _margin) {
            // stop
            didA = true;
            _decelerate(_max_acceleration, dt);
          } else {
            // clear, switch to new lane, don't acclerate yet
            didA = true;
            _position_distance = newDist;
            _position_laneid = newLane;
            _position_roadid = newRoadID;
          }
        }
      } else {
        // traffic light red/yellow, stop
        didA = true;
        _decelerate(_max_acceleration, dt);
      }
    }
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

void Car::setRoute(const std::deque<size_t>& route) { _route = route; }

void Car::_clipVelocity() {
  double speedLimit = _parentSim->getRoad(_position_roadid).getSpeedLimit();
  _velocity = std::max(0.0, _velocity);
  _velocity = std::min(_velocity, speedLimit);
}
