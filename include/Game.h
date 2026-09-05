#pragma once
#include "Simulation.h"
#include "CarPhysical.h"
#include "RoadPhysical.h"
#include "IntersectionPhysical.h"
#include "utility.h"
#include "Layout.h"

#include "SFML/Graphics.hpp"

#include "EditorWindow.h"
#include "SimulateWindow.h"


#include <map>

#include "Layout.h"

CarPhysical* createPhysicalFromDataFile(Game* game, Car* car, RoadPhysical* roadPhysical, std::string dataFilename);

class Game : public sf::Drawable, sf::Transformable{
  public:
  Game(sf::RenderWindow &window);
  Game(std::string filepath, sf::RenderWindow &window );
  ~Game();

  void step(double dt);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  Layout* getLayout() {return &_l;}

  private:

  std::string getLayoutPath(const std::string& filepath);

  Simulation _s;
  Layout _l;
  std::map<size_t, CarPhysical*> _physicalCars;
  EditorWindow _ew;
};