#include "IntersectionPhysical.h"

IntersectionPhysical::IntersectionPhysical(std::unique_ptr<Intersection> intersection,
                                           sf::Vector2f pos, size_t id)
    : _intersection(std::move(intersection)), _pos(pos), _id(id) {}

void IntersectionPhysical::draw(sf::RenderTarget& target,
                                sf::RenderStates states) const {
  // haha do nothing
  // TODO draw something
}
