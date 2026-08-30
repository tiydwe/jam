#include "Simulation.h"

#include "utility.h"

Simulation::Simulation() {}

void Simulation::step(double dt) {
  for(auto& [id, car] : _cars){
    car.move(dt);
  }
}

void Simulation::addCar(const Car& car) {
#ifdef DEBUG
  if (_cars.find(car.getID()) != _cars.end()) {
    utility::logWarn("While trying to addCar, id " +
                     std::to_string(car.getID()) + " already existed.");
    utility::exit();
  }
#endif
  _cars.insert_or_assign(car.getID(), car);
}

void Simulation::addIntersection(const Intersection& intersection) {
#ifdef DEBUG
  if (_intersections.find(intersection.getID()) != _intersections.end()) {
    utility::logWarn("While trying to addIntersection, id " +
                     std::to_string(intersection.getID()) +
                     " already existed.");
    utility::exit();
  }
#endif
  _intersections.insert_or_assign(intersection.getID(), intersection);
}

void Simulation::addLane(const Lane& lane) {
#ifdef DEBUG
  if (_lanes.find(lane.getID()) != _lanes.end()) {
    utility::logWarn("While trying to addLane, id " +
                     std::to_string(lane.getID()) + " already existed.");
    utility::exit();
  }
#endif
  _lanes.insert_or_assign(lane.getID(), lane);
}

void Simulation::addRoad(const Road& road) {
#ifdef DEBUG
  if (_roads.find(road.getID()) != _roads.end()) {
    utility::logWarn("While trying to addRoad, id " +
                     std::to_string(road.getID()) + " already existed.");
    utility::exit();
  }
#endif
  _roads.insert_or_assign(road.getID(), road);
}

Car& Simulation::getCar(size_t id) {
#ifdef DEBUG
  if (_cars.find(id) == _cars.end()) {
    utility::logErr("While trying to getCar, id " + std::to_string(id) +
                    " was not found.");
    utility::exit();
  }
#endif
  return _cars[id];
}

Intersection& Simulation::getIntersection(size_t id) {
#ifdef DEBUG
  if (_intersections.find(id) == _intersections.end()) {
    utility::logErr("While trying to getIntersection, id " +
                    std::to_string(id) + " was not found.");
    utility::exit();
  }
#endif
  return _intersections[id];
}

Lane& Simulation::getLane(size_t id) {
#ifdef DEBUG
  if (_lanes.find(id) == _lanes.end()) {
    utility::logErr("While trying to getLane, id " + std::to_string(id) +
                    " was not found.");
    utility::exit();
  }
#endif
  return _lanes[id];
}

Road& Simulation::getRoad(size_t id) {
#ifdef DEBUG
  if (_roads.find(id) == _roads.end()) {
    utility::logErr("While trying to getRoad, id " + std::to_string(id) +
                    " was not found.");
    utility::exit();
  }
#endif
  return _roads[id];
}
