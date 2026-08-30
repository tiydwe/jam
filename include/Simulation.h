#pragma once

#include <vector>
#include <map>
#include "Car.h"
#include "Intersection.h"
#include "Lane.h"
#include "Road.h"
#include "utility"

class Simulation{
  public:
  Simulation();

  void step(double dt);

  void addCar(const Car& car);
  void addIntersection(const Intersection& intersection);
  void addLane(const Lane& lane);
  void addRoad(const Road& road);

  Car& getCar(size_t id);
  Intersection& getIntersection(size_t id);
  Lane& getLane(size_t id);
  Road& getRoad(size_t id);
  double getTime();

  private:
  
  std::map<size_t, Car> _cars;
  std::map<size_t, Intersection> _intersections;
  std::map<size_t, Lane> _lanes;
  std::map<size_t, Road> _roads; 

  double _time;

};