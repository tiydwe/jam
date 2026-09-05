#include "Simulation.h"

#include <unordered_set>

#include "utility.h"

Simulation::Simulation(unsigned int seed) : _time(0.0), _rng(seed) {}

Simulation::~Simulation() {
    for (const auto& x : _cars)
        delete x.second;
    _cars.clear();

    for (const auto& x : _roads)
        delete x.second;
    _roads.clear();

    for (const auto& x : _intersections)
        delete x.second;
    _intersections.clear();

    for (const auto& x : _lanes)
        delete x.second;
    _lanes.clear();
}
void Simulation::step(double dt) {
  _time += dt;
  for(auto& [id, car] : _cars){
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
  while(!q.empty()){
    size_t curr = q.front();
    q.pop_front();
    if(curr == endRoad){
      break;
    }
    auto& outs = getIntersection(getRoad(curr).getEndIntersection()).getOutgoings();
    for(const size_t nxt : outs){
      if(visited.find(nxt) == visited.end()){
        visited.insert(nxt);
        prev[nxt] = curr;
        q.push_back(nxt);
      }
    }
  }
  std::deque<size_t> res;
  size_t curr = endRoad;
  while(curr != startRoad){
    res.push_front(curr);
    curr = prev[curr];
  }
  return res;
}

void Simulation::addCar(Car* car) {
#ifdef DEBUG
  if (_cars.find(car->getID()) != _cars.end()) {
    utility::logWarn("While trying to addCar, id " +
                     std::to_string(car->getID()) + " already existed.");
    utility::exit();
  }
#endif
  _cars.try_emplace(car->getID(), car);
}

void Simulation::addIntersection(Intersection* intersection) {
#ifdef DEBUG
  if (_intersections.find(intersection->getID()) != _intersections.end()) {
    utility::logWarn("While trying to addIntersection, id " +
                     std::to_string(intersection->getID()) +
                     " already existed.");
    utility::exit();
  }
#endif
  _intersections.try_emplace(intersection->getID(), intersection);
}

void Simulation::addLane(Lane* lane) {
#ifdef DEBUG
  if (_lanes.find(lane->getID()) != _lanes.end()) {
    utility::logWarn("While trying to addLane, id " +
                     std::to_string(lane->getID()) + " already existed.");
    utility::exit();
  }
#endif
  _lanes.try_emplace(lane->getID(), lane);
}

void Simulation::addRoad(Road* road) {
#ifdef DEBUG
  if (_roads.find(road->getID()) != _roads.end()) {
    utility::logWarn("While trying to addRoad, id " +
                     std::to_string(road->getID()) + " already existed.");
    utility::exit();
  }
#endif
  _roads.try_emplace(road->getID(), road);
}

Car& Simulation::getCar(size_t id) {
#ifdef DEBUG
  if (_cars.find(id) == _cars.end()) {
    utility::logErr("While trying to getCar, id " + std::to_string(id) +
                    " was not found.");
    utility::exit();
  }
#endif
  return *_cars[id];
}

Intersection& Simulation::getIntersection(size_t id) {
#ifdef DEBUG
  if (_intersections.find(id) == _intersections.end()) {
    utility::logErr("While trying to getIntersection, id " +
                    std::to_string(id) + " was not found.");
    utility::exit();
  }
#endif
  return *_intersections.at(id);
}

Lane& Simulation::getLane(size_t id) {
#ifdef DEBUG
  if (_lanes.find(id) == _lanes.end()) {
    utility::logErr("While trying to getLane, id " + std::to_string(id) +
                    " was not found.");
    utility::exit();
  }
#endif
  return *_lanes[id];
}

Road& Simulation::getRoad(size_t id) {
#ifdef DEBUG
  if (_roads.find(id) == _roads.end()) {
    utility::logErr("While trying to getRoad, id " + std::to_string(id) +
                    " was not found.");
    utility::exit();
  }
#endif
  return *_roads[id];
}

double Simulation::getTime() { return _time; }

std::mt19937& Simulation::getRNG() {
  return _rng;
}
