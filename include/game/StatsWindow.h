#pragma once

#include <SFML/Graphics.hpp>
#include "Layout.h"
#include "Button.h"
#include "Simulation.h"

#include <memory>
#include <utility>

class Game;

class StatsWindow : public sf::Drawable, sf::Transformable{
 public:
  StatsWindow(Game* game, OverallStats stats, sf::Vector2u windowSize);

  std::string getStatsString();

  void updateWindowSize(sf::Vector2f newSize);

  void update(sf::RenderWindow &rw);

  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;
  
 private:
  Game* _game;

  sf::RectangleShape _container;
  sf::Text _statsText;
  
  Button _continue;

  OverallStats _stats;

};