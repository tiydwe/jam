#pragma once

#include <vector>
#include <map>
#include <deque>
#include <random>
#include "Car.h"
#include "Intersection.h"
#include "Lane.h"
#include "Road.h"
#include "utility"

class Simulation{
  public:
  Simulation(unsigned int seed = std::random_device()());
  ~Simulation();

  void step(double dt);

  std::deque<size_t> findRoute(size_t startRoad, size_t endRoad);

  // MUST BE HEAP ALLOCATED
  void addCar(Car* car);
  void addIntersection(Intersection* intersection);
  void addLane(Lane* lane);
  void addRoad(Road* road);

  Car& getCar(size_t id);
  Intersection& getIntersection(size_t id);
  Lane& getLane(size_t id);
  Road& getRoad(size_t id);
  double getTime();

  std::mt19937& getRNG();


  private:
  
  std::mt19937 _rng;

  std::map<size_t, Car*> _cars;
  std::map<size_t, Intersection*> _intersections;
  std::map<size_t, Lane*> _lanes;
  std::map<size_t, Road*> _roads; 

  double _time;

};