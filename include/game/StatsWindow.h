#pragma once

#include <SFML/Graphics.hpp>
#include "Layout.h"
#include "Button.h"
#include "Simulation.h"

#include <memory>
#include <utility>
#include <filesystem>

class Game;

class Level {
 public:
  Level(std::filesystem::path levelConfigPath);
  bool passes(OverallStats stats);
  double getPercentArrived() const {return _percentArrived;}
  double getAvgSpeed() const {return _avgSpeed;}
  double getTimeWasted() const {return _timeWasted;}

  bool pass1(OverallStats stats) const;
  bool pass2(OverallStats stats) const;

 private:
  double _percentArrived = 0.0;
  double _avgSpeed = 0.0;
  double _timeWasted = 1.0;
};

class StatsWindow : public sf::Drawable, sf::Transformable{
 public:
  StatsWindow(Game* game, OverallStats stats, Level &level, sf::Vector2u windowSize);

  std::string getStatsString();

  void updateWindowSize(sf::Vector2f newSize);

  void update(sf::Vector2f mousePosition, bool enable);

  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;
  
 private:
  Game* _game;

  sf::RectangleShape _container;
  sf::RectangleShape _carsArrived;
  sf::RectangleShape _averageSpeed;
  sf::RectangleShape _background;
  sf::Text _statsText;
  
  Button _continue;

  OverallStats _stats;
  Level& _level;

};