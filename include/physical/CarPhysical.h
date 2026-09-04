#pragma once

#include "Car.h"
#include "RoadPhysical.h"
#include "SFML/Graphics.hpp"

class Game;

/// @brief Handles physical aspects of a car, such as position, drawing, etc.
/// Does not handle any simulation car logic
class CarPhysical : public sf::Drawable, sf::Transformable {
 public:
  CarPhysical(Game* game, Car* car, RoadPhysical* rp, std::string texturePath);

  void update();

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
  Game* getGame() {return _game;}

 private:
  Game* _game = nullptr;
  Car* _car = nullptr;
  RoadPhysical* _road = nullptr;
  sf::Texture _texture;
  sf::Sprite _base;

  sf::Vector2<float> _pos;
};