#include "Car.h"

#include <algorithm>

#include "Simulation.h"
#include "utility.h"

void Car::move(double dt) {
  // Priority #1 is merging
  if (!(_status == carStatus::MERGING)) {
    // random chance to merge into required lane, increases closer to
    // intersection
  }
  if (_status == carStatus::MERGING) {
    _check_merge();
  } else {
    // if car is coming up on desination, slow down
    // TODO implement lane checking
    _minStoppingDist = _velocity * _velocity / 2.0 / _max_acceleration;
    bool didA = false;
    bool foundDestination = false;
    if (_position_roadid == _current_destination.first &&
        _current_destination.second - _position_distance <=
            _minStoppingDist + _margin) {
#ifdef DEBUG
      // utility::log("Target spotted at  " +
      // std::to_string(_position_distance));
#endif
      _decelerate(_max_acceleration, dt);
      didA = true;
      foundDestination = true;
      if (utility::isclose(_current_destination.second, _position_distance,
                           10.0) &&
          _current_destination.first == _position_roadid) {
        _status = carStatus::ARRIVED;
        // remove car after arrival
        _parentSim->getLane(_position_laneid).removeCar(_id);
      } else {
        _status = carStatus::ARRIVING;
      }
    }

    // if car coming up at end of road, check intersection

    if (!didA && _move_checkIntersection(dt)) {
      didA = true;
    }

    // check for cars ahead
    auto nextCar = _parentSim->getLane(_position_laneid).minDistance(_id);
    if (nextCar.second != -1) {
      if (nextCar.first - _position_distance <= _minStoppingDist + _margin && !didA) {
        _decelerate(_max_acceleration, dt);
#ifdef DEBUG
        utility::log(std::to_string(_id) + " - Next car ahead is " +
                     std::to_string(nextCar.second) + " with distance " +
                     std::to_string(nextCar.first));
#endif
        _status = carStatus::WAITING_FOR_NEXT_CAR;
        didA = true;
      }
    }

    // otherwise, accelerate slower
    if (!didA) {
      _accelerate(_max_acceleration * 0.8, dt);
      _status = carStatus::TRAVELING;
    }
    _clipVelocity();
    _mergeToNextLane(_applyVelocity(dt));
  }

#ifdef DEBUG
//*
  if (_status != carStatus::ARRIVED) {
    utility::log(std::to_string(_id) + " - Moved to " +
                 std::to_string(_position_distance) + " on road " +
                 std::to_string(_position_roadid) + " on lane " +
                 std::to_string(_position_laneid) + " with status " +
                 carStatusToString(_status));
  }
//*/
#endif
}

bool Car::_move_checkIntersection(double dt) {
  if (_position_distance + _minStoppingDist + _margin >
      _parentSim->getLane(_position_laneid).getLength()) {
    // coming up on next intersection
    // make sure that route has next road
    if (_route.size() == 0 && _current_destination.first != _position_roadid) {
      utility::logErr(
          "While trying to Car::_move_checkIntersection, route ended without "
          "finding "
          "destination.");
      // behaviour: car will not move without valid route
      _decelerate(_max_acceleration, dt);
      _status = carStatus::NO_ROUTE;
      return true;
    }
    if (_route.size() == 0) {
      // coming up on destination, exit
      return false;
    }
    // make sure that route is in correct format, does not include current
    // road
    if (_route.front() == _position_roadid) {
      utility::logWarn(
          "While trying to Car::_move_checkIntersection, route had current "
          "road, fixing.");
      // fix
      _route.pop_front();
    }
    const auto road = _parentSim->getRoad(_route.front());
    auto intersection = _parentSim->getIntersection(
        _parentSim->getRoad(_position_roadid).getEndIntersection());
    int laneid = intersection.getTrafficLight().getLaneCanTurnOnRoad(
        _position_laneid, road.getID(), _parentSim->getTime());
    if (laneid != -1) {
      // go to next road.
      if (std::find(intersection.getOutgoings().begin(),
                    intersection.getOutgoings().end(),
                    road.getID()) == intersection.getOutgoings().end()) {
        // intersection does not have road
        utility::logErr(
            "While trying to Car::move, intersection did not have road on "
            "route");
        utility::logErr("Outgoings was " +
                        std::to_string(intersection.getOutgoings().size()));

        _decelerate(_max_acceleration, dt);
        _status = carStatus::NO_ROUTE;
        return true;
      } else {
        // TODO: make it so car doesn't just teleport
        double newDist = 0.0;
        size_t newLane = laneid;
        size_t newRoadID = road.getID();
        // make sure there arn't any cars in the way
        auto nxtCar = _parentSim->getLane(newLane).minDistance();
        if (nxtCar.second != -1 &&
            nxtCar.first < newDist + _minStoppingDist + _margin) {
          // stop
          _decelerate(_max_acceleration, dt);
          _status = carStatus::WAITING_FOR_NEXT_CAR;
          return true;

        } else {
          // clear, switch to new lane, don't acclerate yet
          // only switch to next lane if at ned of road
          if (_position_distance >
              _parentSim->getLane(_position_laneid).getLength()) {
            _parentSim->getLane(_position_laneid).removeCar(_id);
            _position_distance = newDist;
            _position_laneid = newLane;
            _position_roadid = newRoadID;
            _route.pop_front();
            _parentSim->getLane(newLane).addCar(newDist, _id);
            _status = carStatus::TRAVELING;
#ifdef DEBUG
            utility::log(std::to_string(_id) +
                         " - Switched at intersection, moved to new lane " +
                         std::to_string(_position_laneid));
#endif
            return true;
          } else {
            // keep on going, all clear
          }
        }
      }
    } else {
      // traffic light red/yellow, stop
      _decelerate(_max_acceleration, dt);
      _status = carStatus::WAITING_AT_INTERSECTION;
      return true;
    }
  }
  return false;
}
