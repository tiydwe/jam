#include "Lane.h"

#include <algorithm>

#include "Simulation.h"
#include "utility.h"

Lane::Lane() {
  utility::exit(
      "This should never be called (probally something messed up in map!)");
}

Lane::Lane(Simulation& parent, double length)
    : _parent(&parent), _id(utility::uid()), _length(length) {}

std::pair<double, size_t> Lane::minDistance() {
  if (_cars.size() == 0) {
    return std::make_pair<double, size_t>(2 * this->_length, -1);
  }
  return (*std::min_element(_cars.begin(), _cars.end(),
                            [](const std::pair<double, size_t>& lhs,
                               const std::pair<double, size_t>& rhs) {
                              return lhs.first < rhs.first;
                            }));
}

std::pair<double, size_t> Lane::minDistance(size_t carid) {
  std::pair<double, size_t> res{-1.0, -1};
  for (const auto& car : _cars) {
    if (car.first > _parent->getCar(carid).getDistance()) {
      if (res.first == -1) {
        res = car;
      } else {
        res = max(res, car);
      }
    }
  }
  return res;
}

void Lane::removeCar(size_t car) {
  for (auto it = _cars.begin(); it < _cars.end();) {
    if (it->second == car) {
      it =  _cars.erase(it);
    } else {
      ++it;
    }
  }
}

void Lane::moveCar(double newDist, size_t car) {
  for (auto it = _cars.begin(); it < _cars.end();) {
    if (it->second == car) {
      it->first = newDist;
      ++it;
    } else {
      ++it;
    }
  }
}
