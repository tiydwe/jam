#pragma once

#include "Car.h"
#include "RoadPhysical.h"
#include "SFML/Graphics.hpp"

#include <memory>
#include <utility>

class Simulation;

/// @brief Handles physical aspects of a car, such as position, drawing, etc.
/// Does not handle any simulation car logic
class CarPhysical : public sf::Drawable, sf::Transformable {
 public:
  CarPhysical(Simulation* game, std::unique_ptr<Car> car, RoadPhysical* rp, std::string texturePath);

  void move(double dt);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
  Simulation* getGame() {return _game;}
  Car* getCar() const {return _car.get();}

 private:
  Simulation* _game = nullptr;
  std::unique_ptr<Car> _car;
  RoadPhysical* _road = nullptr;
  sf::Texture _texture;
  sf::Sprite _base;

  sf::Vector2<float> _pos;
};