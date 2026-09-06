#include "IntersectionPhysical.h"

IntersectionPhysical::IntersectionPhysical(std::unique_ptr<Intersection> intersection,
                                           sf::Vector2f pos)
    : _intersection(std::move(intersection)), _pos(pos) {}

void IntersectionPhysical::draw(sf::RenderTarget& target,
                                sf::RenderStates states) const {
  // haha do nothing
  // TODO draw something
}
