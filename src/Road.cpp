#include "Road.h"

#include <algorithm>
#include <cmath>

#include "Simulation.h"
#include "utility.h"

Road::Road() {
  utility::exit(
      "This should never be called (probally something messed up in map!)");
}

Road::Road(Simulation& parent, double speedLimit, size_t endIntersection, double angle)
    : _parent(&parent),
      _id(utility::uid()),
      _speed_limit(speedLimit),
      _endIntersection(endIntersection),
      _angle(angle) {}

int Road::laneToGoTo(size_t sourceLane, size_t targetLane) {
  auto targetIt = std::find(_lanes.begin(), _lanes.end(), targetLane);
  auto sourceIt = std::find(_lanes.begin(), _lanes.end(), sourceLane);
  if (targetIt == _lanes.end()) {
    utility::logWarn("Road::laneToGoTo - Target lane " +
                     std::to_string(targetLane) + " not found.");
    return -2;
  }
  if (sourceIt == _lanes.end()) {
    utility::logWarn("Road::laneToGoTo - Source lane " +
                     std::to_string(sourceLane) + " not found.");
    return -2;
  }
  if (sourceIt > targetIt) {
    return *(sourceIt - 1);
  } else if (sourceIt < targetIt) {
    return *(sourceIt + 1);
  } else {
    return -1;
  }
}

int Road::closestLaneTo(size_t lane, const std::vector<size_t>& lanes) {
  auto sourceIt = std::find(_lanes.begin(), _lanes.end(), lane);
  if (sourceIt == _lanes.end()) {
    utility::logWarn("Road::laneToGoTo - Source lane " + std::to_string(lane) +
                     " not found.");
    return -2;
  }
  size_t bestLane = -1;
  size_t bestDist = 1000000;
  for (auto target : lanes) {
    auto targetIt = std::find(_lanes.begin(), _lanes.end(), target);
    if (targetIt == _lanes.end()) {
      utility::logWarn("Road::laneToGoTo - Target lane " +
                       std::to_string(target) + " not found.");
      return -2;
    }
    auto currDist = static_cast<long>(std::distance(targetIt, sourceIt));
    if( abs(currDist) < bestDist){
      bestLane = *targetIt;
      bestDist = currDist;
    }
  }
  return bestLane;
}

int Road::getLanePosFromCenter(size_t laneid) const {
  auto it = _lanes.begin();
  for(int i = 0; i < _lanes.size(); ++i){
    if(*it == laneid){
      return i;
    }
    ++it;
  }
  utility::logErr("Road::getLanePosFromCenter - laneid not found");
  utility::exit();
  return 0;
}
