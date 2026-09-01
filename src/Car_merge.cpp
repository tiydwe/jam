#include <random>

#include "Car.h"
#include "Simulation.h"
#include "utility.h"

bool Car::_mergeToNextLane(double dd) {
  std::uniform_real_distribution<double> dis(0.0, 1.0);
  double dperc = _position_distance /
                 (_parentSim->getLane(_position_laneid).getLength() - _margin);
  double chance = utility::cumlitivePtoSlice(utility::smoothstep, dperc, dd/(_parentSim->getLane(_position_laneid).getLength() - _margin));
  if (dis(_parentSim->getRNG()) <= chance) {
    if (_route.size() == 0) {
      // probally at destination
      return false;
    }
    auto road = _parentSim->getRoad(_position_roadid);
    int nextLane = road.closestLaneTo(
        _position_laneid, _parentSim->getIntersection(road.getEndIntersection())
                              .getTrafficLight()
                              .whereToTurn(_route.front()));
    if(nextLane == _position_laneid){
      return false;
    }
    if (nextLane < 0) {
      // err
      utility::logErr(
          "Car::_mergeToNextLane got negative return from closestLaneTo");
    } else {
      _init_merge(nextLane);
      return true;
    }
  }
  return false;
}

void Car::_init_merge(size_t newLaneid) {
  // create new phantom car in new lane
  // wait until traffic clears around phantom car (engouth space)
  // move to new lane, remove old car
  // this means that the car will block 2 lanes for a time (realistic)

  // WARNING this might break, assuming distance is equal (+ margin)
  if (_status == carStatus::MERGING) {
    utility::logWarn("Car::_init_merge was called but car is already merging!");
    return;
  }
  _status = carStatus::MERGING;
  _newLaneMerge = newLaneid;
  _newMergedDistance = _position_distance + _margin;
  _parentSim->getLane(newLaneid).addCar(_newMergedDistance, _id);
}

bool Car::_check_merge() {
  if (_status != carStatus::MERGING) {
    utility::logWarn("Car::_check_merge was called but car is not merging!");
    return false;
  }
  auto minD = _parentSim->getLane(_newLaneMerge).minDistance(_id);
  if (minD.second == -1 || (minD.first - _newMergedDistance) > _margin) {
    // clear to merge
    _parentSim->getLane(_position_laneid).removeCar(_id);
    _position_distance = _newMergedDistance;
    _position_laneid = _newLaneMerge;
    _status = carStatus::TRAVELING;
    return true;
  } else {
    // not clear
    return false;
  }
}
