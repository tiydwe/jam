#pragma once

#include "Car.h"
#include "RoadPhysical.h"
#include "SFML/Graphics.hpp"

/// @brief Handles physical aspects of a road, such as position, drawing, etc.
/// Does not handle any simulation road logic
class CarPhysical : public sf::Drawable, sf::Transformable {
 public:
  CarPhysical(Car* car, RoadPhysical* rp, std::string texturePath);

  void update();

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 private:
  const Car* _car = nullptr;
  const RoadPhysical* _road = nullptr;
  sf::Texture _texture;
  sf::Sprite _base;

  sf::Vector2<float> _pos;
};