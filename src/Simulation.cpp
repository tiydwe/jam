#include "Simulation.h"

#include <fstream>
#include <istream>
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
  for (auto& [id, car] : _cars) {
    car->move(dt);
  }
}

std::deque<size_t> Simulation::findRoute(size_t startRoad, size_t endRoad) {
  // loc, prev
  std::map<size_t, size_t> prev;
  std::deque<size_t> q;
  q.push_back(startRoad);
  std::unordered_set<size_t> visited;
  visited.insert(startRoad);
  while (!q.empty()) {
    size_t curr = q.front();
    q.pop_front();
    if (curr == endRoad) {
      break;
    }
    auto& outs =
        _layout
            ->getIntersectionFromInternalID(_layout->getRoad(curr)->getEndIntersection())
            ->getOutgoings();
    for (const size_t nxt : outs) {
      if (visited.find(nxt) == visited.end()) {
        visited.insert(nxt);
        prev[nxt] = curr;
        q.push_back(nxt);
      }
    }
  }
  std::deque<size_t> res;
  size_t curr = endRoad;
  while (curr != startRoad) {
    res.push_front(curr);
    if (prev[curr] == curr) {
      // no route
      //utility::logWarn("Simulation::findRoute - Route not found!");
      return {};
    }
    curr = prev[curr];
  }
  return res;
}

void Simulation::removeCar(size_t internalid) {
  CarPhysical* cr = nullptr;
  auto it = _cars.begin();
  while(it != _cars.end()){
    if(it->second->getCar()->getID() == internalid){
      cr = it->second.get();
      _overall.addStat(cr->getCar()->getResults());
      _carsDone.try_emplace(it->first, std::move(it->second));
      it = _cars.erase(it);
    }
    else{
      ++it;
    }
  }
  if(cr){
    this->getLane(cr->getCar()->getCurrLane())->removeCar(cr->getCar()->getID());
  }
}

void Simulation::addCar(std::unique_ptr<CarPhysical> car) {
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

OverallStats Simulation::getStats()  {
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
