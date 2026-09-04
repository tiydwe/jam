#pragma once

#include "Intersection.h"
#include "RoadPhysical.h"
#include "SFML/Graphics.hpp"

/// @brief Handles physical aspects of a road, such as position, drawing, etc.
/// Does not handle any simulation road logic
class IntersectionPhysical : public sf::Drawable, sf::Transformable {
 public:
  IntersectionPhysical(Intersection* intersection, sf::Vector2f pos);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  sf::Vector2f getPos() const {return _pos;}
  size_t getInternalID() const {return _intersection->getID();}

 private:
  const Intersection* _intersection = nullptr;

  sf::Vector2<float> _pos;
};