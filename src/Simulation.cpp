#include "Simulation.h"

#include <fstream>
#include <functional>
#include <istream>
#include <numeric>
#include <queue>
#include <sstream>
#include <unordered_set>

#include "Layout.h"
#include "utility.h"

std::unique_ptr<CarPhysical> createPhysicalFromDataFile(
    std::unique_ptr<Car> car, RoadPhysical* roadPhysical,
    Simulation* simulation, std::string dataFilename) {
  std::ifstream file(dataFilename);
  if (!file.is_open()) {
    utility::logErr("Error opening data filename " + dataFilename);
    return nullptr;
  }
  std::string tmp;
  std::getline(file, tmp);
  std::getline(file, tmp);
  return std::make_unique<CarPhysical>(simulation, std::move(car), roadPhysical,
                                       tmp);
}

Simulation::Simulation(Layout* layout, std::string filepath, unsigned int seed)
    : _layout(layout), _time(0.0), _rng(seed) {
  for (const auto& x : _layout->getLanes()) {
    _simulationLanes.try_emplace(
        x.first, std::move(std::make_unique<SimulationLane>(this, x.second)));
  }
  std::ifstream file(filepath);
  if (!file.is_open()) {
    utility::logErr("Error opening game file! " + filepath);
    return;
  }
  std::string tmp;
  while (std::getline(file, tmp)) {
    std::stringstream ss(tmp);
    std::string type;
    ss >> type;
    if (type == "C") {
      size_t start, end;
      char lrs, lre;
      double initDist, finDist, maxa;
      std::string datafilename;
      ss >> datafilename >> start >> lrs >> end >> lre >> initDist >> finDist >>
          maxa;
      std::unique_ptr<Car> cp;
      if (lrs == 'l') {
        cp = std::make_unique<Car>(
            *this, _layout->getPhysicalRoad(start)->getInternalIDL(),
            _layout->getPhysicalRoad(start)->getRoadL()->getEdgeLane(),
            initDist, maxa);
      } else {
        cp = std::make_unique<Car>(
            *this, _layout->getPhysicalRoad(start)->getInternalIDR(),
            _layout->getPhysicalRoad(start)->getRoadR()->getEdgeLane(),
            initDist, maxa);
      }
      _simulationLanes[cp->getCurrLane()]->addCar(cp->getCurrDist(),
                                                  cp->getID());
      if (lre == 'l') {
        cp->setDestination(
            {_layout->getPhysicalRoad(end)->getInternalIDL(), finDist});
      } else {
        cp->setDestination(
            {_layout->getPhysicalRoad(end)->getInternalIDR(), finDist});
      }
      cp->recalcRoute();
      size_t oldid = cp->getID();
      std::unique_ptr<CarPhysical> cpp = createPhysicalFromDataFile(
          std::move(cp), _layout->getPhysicalRoad(start), this, datafilename);
      _cars.try_emplace(oldid, std::move(cpp));
    } else if (type == "#") {
      continue;
    } else {
      break;
    }
  }
}

Simulation::~Simulation() {}

void Simulation::step(double dt) {
  _time += dt;
  for (auto it = _cars.begin(); it != _cars.end();) {
    it->second->move(dt);
    if (_needsFixing) {
      it = _fixDelete;
      _needsFixing = false;
    } else {
      ++it;
    }
  }
}

std::deque<size_t> Simulation::findRoute(size_t startRoad, size_t endRoad) {
  // loc, prev
  std::map<size_t, size_t> prev;
  std::map<size_t, double> dist;
  std::priority_queue<std::pair<double, size_t>,
                      std::vector<std::pair<double, size_t>>,
                      std::greater<std::pair<double, size_t>>>
      q;
  for (const auto& x : _layout->getPhysicalIntersections()) {
    dist[x.second->getInternalID()] = std::numeric_limits<double>::infinity();
    prev[x.second->getInternalID()] = std::numeric_limits<size_t>::max();
  }
  auto startIntersection = _layout->getRoad(startRoad)->getEndIntersection();
  dist[startIntersection] = 0;
  q.push({0.0, startIntersection});
  auto endIntersection =
      _layout->getBeginIntersectionFromInternalRoadID(endRoad);
  if (endIntersection == nullptr) {
    return {};
  }
  while (!q.empty()) {
    auto curr = q.top();
    q.pop();
    if (curr.second == endIntersection->getInternalID()) {
      break;
    }
    auto& outs =
        _layout->getIntersectionFromInternalID(curr.second)->getOutgoings();
    for (const size_t nxt : outs) {
      size_t nexti = _layout->getRoad(nxt)->getEndIntersection();
      if (std::isfinite(dist[curr.second])) {
        double newDist =
            dist[curr.second] +
            _layout->getPhysicalRoadFromInternalID(nxt)->getLength();
        if (newDist < dist[nexti]) {
          dist[nexti] = newDist;
          prev[nexti] = curr.second;
          q.push({newDist, nexti});
        }
      }
    }
  }
  std::deque<size_t> res = {endRoad};
  size_t curr = endIntersection->getInternalID();
  size_t previ = prev[curr];
  while (curr != startIntersection) {
    if (prev[curr] == std::numeric_limits<size_t>::max()) {
      return {};  // no route
    }
    size_t previ = prev[curr];
    auto road =
        _layout->getRoadBetweenTwoIntersectionsFromInternalID(previ, curr);
    if (road == nullptr) {
      return {};
    }
    res.push_front(road->getID());
    curr = previ;
  }

  return res;
}

void Simulation::removeCar(size_t internalid) {
  CarPhysical* cr = nullptr;
  auto it = _cars.begin();
  while (it != _cars.end()) {
    if (it->second->getCar()->getID() == internalid) {
      cr = it->second.get();
      _overall.addStat(cr->getCar()->getResults());
      _carsDone.try_emplace(it->first, std::move(it->second));
      it = _cars.erase(it);
      _needsFixing = true;
      _fixDelete = it;
    } else {
      ++it;
    }
  }
  if (cr) {
    this->getLane(cr->getCar()->getCurrLane())
        ->removeCar(cr->getCar()->getID());
  }
}

void Simulation::addCar(std::unique_ptr<CarPhysical> car) {
  _simulationLanes[car->getCar()->getCurrLane()]->addCar(
      car->getCar()->getCurrDist(), car->getCar()->getID());
  _cars.try_emplace(car->getCar()->getID(), std::move(car));
}

Car* Simulation::getCar(size_t id) const {
#ifdef DEBUG
  if (_cars.find(id) == _cars.end()) {
    utility::logErr("While trying to getCar, id " + std::to_string(id) +
                    " was not found.");
    utility::exit();
  }
#endif
  return _cars.at(id)->getCar();
}

SimulationLane* Simulation::getLane(size_t id) const {
#ifdef DEBUG
  if (_simulationLanes.find(id) == _simulationLanes.end()) {
    utility::logErr("While trying to getLane, id " + std::to_string(id) +
                    " was not found.");
    utility::exit();
  }
#endif
  return _simulationLanes.at(id).get();
}

double Simulation::getTime() { return _time; }

OverallStats Simulation::getStats() {
  _overall.numberOfCars = _cars.size() + _carsDone.size();
  return _overall;
}

bool Simulation::isDone() const { return _cars.empty(); }

std::mt19937& Simulation::getRNG() { return _rng; }

void Simulation::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  _layout->draw(target, states);
  for (const auto& x : _cars) {
    x.second->draw(target, states);
  }
}

void OverallStats::addStat(ResultStats result) {
  numberOfCars++;
  numberArrived += result._arrived;
  totalTimeTraveled += result._timeToArrival;
  totalDistanceTraveled += result._distanceTravled;
  totalTimeAtIntersection += result._timeWastedAtIntersection;
  totalTimeWaitingNextCar += result._timeWastedForNextCar;
}
