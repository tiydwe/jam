#include "TrafficLight.h"

#include <algorithm>
#include <cmath>
#include <map>

#include "Simulation.h"
#include "utility.h"

RoadWrapper::RoadWrapper(Simulation& s, size_t roadid)
    : _parent(&s), _roadid(roadid) {}

bool RoadWrapper::operator<(const RoadWrapper& r) const {
  return _parent->getRoad(_roadid).getAngle() <
         _parent->getRoad(r.getRoadID()).getAngle();
}

TrafficLight::TrafficLight(Simulation& s) : _parent(&s), _totalLanes(0) {}

bool TrafficLight::addIngoing(size_t roadid) {
  if (_totalLanes + _parent->getRoad(roadid).getNumLanes() > MAX_LANES) {
    utility::logWarn("TrafficLight::addIngoing - max lanes reached");
    return false;
  }
#ifdef DEBUG
  // defensive
  bool inOut = false;
  for (auto it = _outgoingRoads.begin(); it != _outgoingRoads.end(); ++it) {
    if (it->getRoadID() == roadid) {
      inOut = true;
    }
  }
  if (inOut) {
    utility::logWarn("TrafficLight::addIngoing - attempted to add " +
                     std::to_string(roadid) +
                     " but was already in outgoing. Not adding to ingoing.");
    return false;
  }
#endif
  _ingoingRoads.insert(RoadWrapper(*_parent, roadid));
  _totalLanes += _parent->getRoad(roadid).getNumLanes();
  return true;
}

bool TrafficLight::addOutgoing(size_t roadid) {
  if (_totalLanes + _parent->getRoad(roadid).getNumLanes() > MAX_LANES) {
    utility::logWarn("TrafficLight::addOutgoing - max lanes reached");
    return false;
  }
#ifdef DEBUG
  // defensive
  bool inIn = false;
  for (auto it = _ingoingRoads.begin(); it != _ingoingRoads.end(); ++it) {
    if (it->getRoadID() == roadid) {
      inIn = true;
    }
  }
  if (inIn) {
    utility::logWarn("TrafficLight::addOutgoing - attempted to add " +
                     std::to_string(roadid) +
                     " but was already in ingoing. Not adding to outgoing.");
    return false;
  }
#endif
  _outgoingRoads.insert(RoadWrapper(*_parent, roadid));
  _totalLanes += _parent->getRoad(roadid).getNumLanes();
  return true;
}

void TrafficLight::removeRoad(size_t roadid) {
  bool inIn = false;
  bool inOut = false;
  for (auto it = _ingoingRoads.begin(); it != _ingoingRoads.end(); ++it) {
    if (it->getRoadID() == roadid) {
      it = _ingoingRoads.erase(it);
      inIn = true;
    }
  }
  for (auto it = _outgoingRoads.begin(); it != _outgoingRoads.end(); ++it) {
    if (it->getRoadID() == roadid) {
      it = _outgoingRoads.erase(it);
      inOut = true;
    }
  }
  if (!inIn && !inOut) {
    utility::logWarn("TrafficLight::removeRoad - attempted to remove roadid " +
                     std::to_string(roadid) + " but was not found.");
  } else if (inIn && inOut) {
    utility::logWarn("TrafficLight::removeRoad - attempted to remove roadid " +
                     std::to_string(roadid) +
                     " but was found in both in and out.");
  }
  for (ScheduleItem& si : _schedule) {
    for (const auto& [path, light] : si.valid) {
      if (_parent->getLane(path.first).getRoad() == roadid ||
          _parent->getLane(path.second).getRoad() == roadid) {
        si.valid.erase(path);
      }
    }
  }
}

size_t TrafficLight::timeToScheduleItem(double time) {
  double totalCycleTime = 0;
  for (ScheduleItem& s : _schedule) {
    totalCycleTime += s.duration;
  }
  double rem = fmodl(time, totalCycleTime);
  for (int i = 0; i < _schedule.size(); i = (i + 1) % _schedule.size()) {
    if (_schedule[i].duration > rem) {
      return i;
    } else {
      rem -= _schedule[i].duration;
    }
  }
  utility::logErr(
      "TrafficLight::timeToScheduleItem - reached unreacable state.");
  utility::exit();
  return -1;
}

std::vector<size_t> TrafficLight::validTurns(size_t laneid, double time) {
  size_t currentScheduleItem = timeToScheduleItem(time);
  std::vector<size_t> res;
  for (const auto& [path, light] : _schedule[currentScheduleItem].valid) {
    if (path.first == laneid) {
      res.push_back(path.second);
    }
  }
  return res;
}

std::vector<size_t> TrafficLight::whereToTurn(size_t roadid) {
  std::vector<size_t> res;
  for (const ScheduleItem& s : _schedule) {
    for (const auto& [path, light] : s.valid) {
      if (_parent->getLane(path.second).getRoad() == roadid && light == Lights::GREEN) {
        res.push_back(path.first);
      }
    }
  }
  return res;
}

Lights TrafficLight::getLightToLane(size_t laneidSource, size_t laneidTarget,
                                    double time) {
  return _schedule[timeToScheduleItem(time)].valid.at(
      {laneidSource, laneidTarget});
}

int TrafficLight::getLaneCanTurnOnRoad(size_t laneidSource,
                                          size_t roadidTarget, double time) {
  size_t currScheduleItem = timeToScheduleItem(time);
  for (const auto& [path, light] : _schedule[currScheduleItem].valid) {
    if (path.first == laneidSource &&
        _parent->getLane(path.second).getRoad() == roadidTarget &&
      light == Lights::GREEN) {
      return path.second;
    }
  }
  return -1;
}

void TrafficLight::reSchedule(double greenPhaseTime, double yellowPhaseTime) {
  _schedule.clear();
  size_t numOutgoing = 0;
  for (RoadWrapper rw : _outgoingRoads) {
    numOutgoing += _parent->getRoad(rw.getRoadID()).getNumLanes();
  }
  for (RoadWrapper rw : _ingoingRoads) {
    ScheduleItem greenPhase;
    ScheduleItem yellowPhase;
    greenPhase.duration = greenPhaseTime;
    yellowPhase.duration = yellowPhaseTime;
    auto lanes = _parent->getRoad(rw.getRoadID()).getLanes();
    size_t numIn = _parent->getRoad(rw.getRoadID()).getNumLanes();
    size_t baseLanesPerLane = numOutgoing / numIn;
    int extra = numOutgoing - baseLanesPerLane * numIn;
    // this is probally the worst way to move current lane and remaining lanes
    // i love making hard-to-maintain codebases
    size_t currentLane = lanes.back();
    lanes.pop_back();
    // yum cursed c++ -->
    // i love making hard-to-maintain codebases
    int remainingLanes = baseLanesPerLane + (extra --> 0 ? 1 : 0);
    for (RoadWrapper rwo : _outgoingRoads) {
      for (size_t lane : _parent->getRoad(rwo.getRoadID()).getLanes()) {
        if (remainingLanes-- > 0) {
          greenPhase.valid[{currentLane, lane}] = Lights::GREEN;
          yellowPhase.valid[{currentLane, lane}] = Lights::YELLOW;
        } else {
          // no more lanes, move to next
          remainingLanes = baseLanesPerLane + (extra --> 0 ? 1 : 0);
          currentLane = lanes.back();
          lanes.pop_back();
          greenPhase.valid[{currentLane, lane}] = Lights::GREEN;
          yellowPhase.valid[{currentLane, lane}] = Lights::YELLOW;
        }
      }
    }
    _schedule.push_back(greenPhase);
    _schedule.push_back(yellowPhase);
  }
  if (_schedule.size() == 2) {
    // no point in yellow phase
    _schedule.pop_back();
  }
  normalizeSchedule();
}

void TrafficLight::normalizeSchedule() {
  for (ScheduleItem& si : _schedule) {
    // ingoing -> outgoing and outgoing -> ingoing
    for (RoadWrapper rwi : _ingoingRoads) {
      for (RoadWrapper rwo : _outgoingRoads) {
        for (const size_t& la : _parent->getRoad(rwi.getRoadID()).getLanes()) {
          for (const size_t& lb :
               _parent->getRoad(rwo.getRoadID()).getLanes()) {
            si.valid[{lb, la}] = Lights::INVALID;
            if (si.valid.find({la, lb}) == si.valid.end()) {
              si.valid[{la, lb}] = Lights::RED;
            }
          }
        }
      }
    }
    // ingoing -> ingoing
    for (RoadWrapper rwi : _ingoingRoads) {
      for (RoadWrapper rwi2 : _ingoingRoads) {
        for (const size_t& la : _parent->getRoad(rwi.getRoadID()).getLanes()) {
          for (const size_t& lb :
               _parent->getRoad(rwi2.getRoadID()).getLanes()) {
            si.valid[{lb, la}] = Lights::INVALID;
            si.valid[{la, lb}] = Lights::INVALID;
          }
        }
      }
    }
    // outgoing -> outgoing
    for (RoadWrapper rwo : _outgoingRoads) {
      for (RoadWrapper rwo2 : _outgoingRoads) {
        for (const size_t& la : _parent->getRoad(rwo.getRoadID()).getLanes()) {
          for (const size_t& lb :
               _parent->getRoad(rwo2.getRoadID()).getLanes()) {
            si.valid[{lb, la}] = Lights::INVALID;
            si.valid[{la, lb}] = Lights::INVALID;
          }
        }
      }
    }
  }
}
