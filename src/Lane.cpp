#include "Lane.h"

#include <algorithm>

#include "Simulation.h"
#include "utility.h"

Lane::Lane() {
  utility::exit(
      "This should never be called (probally something messed up in map!)");
}

Lane::Lane(Layout* parent, double length, size_t road)
    : _parent(parent), _id(utility::uid()), _length(length), _roadid(road) {}

SimulationLane::SimulationLane(Simulation* parentSim, Lane* lane) : _lane(lane), _parentSim(parentSim) {}

std::pair<double, size_t> SimulationLane::minDistance() {
  if (_cars.size() == 0) {
    return std::make_pair<double, size_t>(2 * this->_lane->getLength(), -1);
  }
  return (*std::min_element(_cars.begin(), _cars.end(),
                            [](const std::pair<double, size_t>& lhs,
                               const std::pair<double, size_t>& rhs) {
                              return lhs.first < rhs.first;
                            }));
}

std::pair<double, size_t> SimulationLane::minDistance(size_t carid) {
  std::pair<double, size_t> res{-1.0, -1};
  for (const auto& car : _cars) {
    if (car.second != carid && car.first > _parentSim->getCar(carid)->getDistance()) {
      if (res.first == -1) {
        res = car;
      } else {
        res = min(res, car);
      }
    }
  }
  return res;
}

void SimulationLane::removeCar(size_t car) {
  for (auto it = _cars.begin(); it < _cars.end();) {
    if (it->second == car) {
      it =  _cars.erase(it);
    } else {
      ++it;
    }
  }
}

void SimulationLane::moveCar(double newDist, size_t car) {
  for (auto it = _cars.begin(); it < _cars.end();) {
    if (it->second == car) {
      it->first = newDist;
      ++it;
    } else {
      ++it;
    }
  }
}
