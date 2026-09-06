#pragma once

#include "Intersection.h"
#include "RoadPhysical.h"
#include "SFML/Graphics.hpp"

#include <memory>
#include <utility>

/// @brief Handles physical aspects of a road, such as position, drawing, etc.
/// Does not handle any simulation road logic
class IntersectionPhysical : public sf::Drawable, sf::Transformable {
 public:
  IntersectionPhysical(std::unique_ptr<Intersection> intersection, sf::Vector2f pos);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  sf::Vector2f getPos() const {return _pos;}
  size_t getInternalID() const {return _intersection->getID();}
  Intersection* getIntersection() const {return _intersection.get();}

 private:
  std::unique_ptr<Intersection> _intersection;

  sf::Vector2<float> _pos;
};