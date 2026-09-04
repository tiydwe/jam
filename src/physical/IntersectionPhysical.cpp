#include "IntersectionPhysical.h"

IntersectionPhysical::IntersectionPhysical(Intersection* intersection,
                                           sf::Vector2f pos)
    : _intersection(intersection), _pos(pos) {}

void IntersectionPhysical::draw(sf::RenderTarget& target,
                                sf::RenderStates states) const {
  // haha do nothing
  // TODO draw something
}
