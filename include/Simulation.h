#pragma once

#include <vector>
#include <map>
#include <deque>
#include <random>
#include "Car.h"
#include "CarPhysical.h"
#include "Intersection.h"
#include "Lane.h"
#include "Road.h"
#include "utility"

#include <SFML/Graphics.hpp>

#include <memory>
#include <utility>
#include <string>



std::unique_ptr<CarPhysical> createPhysicalFromDataFile(
    std::unique_ptr<Car> car, RoadPhysical* roadPhysical,
    Simulation* simulation, std::string dataFilename);


class Simulation : public sf::Drawable, sf::Transformable{
  public:
  Simulation(Layout* layout, std::string filepath, unsigned int seed = std::random_device()());
  ~Simulation();

  void step(double dt);

  std::deque<size_t> findRoute(size_t startRoad, size_t endRoad);

  void addCar(std::unique_ptr<CarPhysical> car);

  SimulationLane* getLane(size_t id) const;
  Car* getCar(size_t id) const;
  Layout* getLayout() const {return _layout;}
  double getTime();

  std::mt19937& getRNG();

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


  private:
  Layout* _layout;
  
  std::mt19937 _rng;

  std::map<size_t, std::unique_ptr<CarPhysical>> _cars;
  std::map<size_t, std::unique_ptr<SimulationLane>> _simulationLanes;

  double _time;

};