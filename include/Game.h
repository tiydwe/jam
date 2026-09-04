#pragma once
#include "Simulation.h"
#include "CarPhysical.h"
#include "RoadPhysical.h"
#include "IntersectionPhysical.h"
#include "utility.h"

#include "SFML/Graphics.hpp"

#include <map>

class Layout{
  public:
  Layout() = default;
  Layout(std::string filepath, Simulation* s);
  ~Layout();

  RoadPhysical* getFromInternalRoadID(size_t internalRoadID);
  
  Simulation* _s;
  std::map<size_t, RoadPhysical*> _physicalRoads;
  std::map<size_t, IntersectionPhysical*> _physicalIntersections;
};

CarPhysical* createPhysicalFromDataFile(Car* car, RoadPhysical* roadPhysical, std::string dataFilename);

class Game : public sf::Drawable, sf::Transformable{
  public:
  Game();
  Game(std::string filepath);
  ~Game();

  void step(double dt);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  Layout* getLayout() {return &_l;}

  private:
  Simulation _s;
  Layout _l;
  std::map<size_t, CarPhysical*> _physicalCars;
};