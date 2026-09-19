#include "IntersectionPhysical.h"

#include <set>
#include <algorithm>
#include <vector>

#include "utility.h"

IntersectionPhysical::IntersectionPhysical(
    std::unique_ptr<Intersection> intersection, sf::Vector2f pos, size_t id)
    : _intersection(std::move(intersection)), _pos(pos), _id(id) {}

IntersectionPhysical::~IntersectionPhysical() {}

void IntersectionPhysical::draw(sf::RenderTarget& target,
                                sf::RenderStates states) const {
  states.transform *= getTransform();

  std::vector<sf::Vector2f> verticies;
  std::set<size_t> idsdone;
  for (const auto& x : _intersection->getRoads()) {
    if(idsdone.find(x.first->getRoadPhysical()->getID()) != idsdone.end()){
      continue;
    }
    idsdone.insert(x.first->getRoadPhysical()->getID());
    auto offr = x.first->getRoadPhysical()->getOffestVectorR();
    offr *= ((offr.length() + 0.5f) / offr.length());
    auto offl = x.first->getRoadPhysical()->getOffestVectorL();
    offl *= ((offl.length() + 0.5f) / offl.length());
    if (x.second == EdgeType::INGOING) {
      verticies.push_back(x.first->getRoadPhysical()->getEnd() + offr);
      verticies.push_back(x.first->getRoadPhysical()->getEnd() + offl);
    } else {
      verticies.push_back(x.first->getRoadPhysical()->getStart() + offr);
      verticies.push_back(x.first->getRoadPhysical()->getStart() + offl);
    }
  }
  /*
  for (auto it = verticies.begin(); it != verticies.end(); ++it) {
    for (auto it2 = verticies.begin(); it2 != verticies.end(); ++it2) {
      // basically same point
      if ((*it - *it2).length() < 0.1f) {
        it2 = verticies.erase(it2);
      }
    }
  }
  //*/
  std::sort(verticies.begin(), verticies.end(),
            [this](const sf::Vector2f& a, const sf::Vector2f& b) {
              float angleA = std::atan2(a.y - this->_pos.y, a.x - this->_pos.x);
              float angleB = std::atan2(b.y - this->_pos.y, b.x - this->_pos.x);
              return angleA < angleB;
            });
  if (verticies.size() >= 3) {
    sf::ConvexShape polygon(verticies.size());
    for (int i = 0; i < verticies.size(); ++i) {
      polygon.setPoint(i, verticies[i]);
    }
    polygon.setFillColor(sf::Color(70, 70, 70));
    target.draw(polygon);
  }
}
