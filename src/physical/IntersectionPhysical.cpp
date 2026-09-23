#include "IntersectionPhysical.h"

#include <algorithm>
#include <set>
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
  std::vector<std::pair<sf::Vector2f, sf::Vector2f>> startends;
  std::set<size_t> idsdone;
  for (const auto& x : _intersection->getRoads()) {
    if (idsdone.find(x.first->getRoadPhysical()->getID()) != idsdone.end()) {
      continue;
    }
    idsdone.insert(x.first->getRoadPhysical()->getID());
    auto offr = x.first->getRoadPhysical()->getOffestVectorR();
    offr *= ((offr.length() + 0.5f) / offr.length());
    auto offl = x.first->getRoadPhysical()->getOffestVectorL();
    offl *= ((offl.length() + 0.5f) / offl.length());
    auto offr2 = x.first->getRoadPhysical()->getOffestVectorR();
    offr2 *= ((offr2.length() - 1.f) / offr2.length());
    auto offl2 = x.first->getRoadPhysical()->getOffestVectorL();
    offl2 *= ((offl2.length() - 1.f) / offl2.length());
    if (x.second == EdgeType::INGOING) {
      verticies.push_back(x.first->getRoadPhysical()->getEnd() + offr);
      verticies.push_back(x.first->getRoadPhysical()->getEnd() + offl);
      startends.push_back({x.first->getRoadPhysical()->getEnd() + offr2,
                    x.first->getRoadPhysical()->getEnd() + offl2});
    } else {
      verticies.push_back(x.first->getRoadPhysical()->getStart() + offr);
      verticies.push_back(x.first->getRoadPhysical()->getStart() + offl);
      startends.push_back({x.first->getRoadPhysical()->getStart() + offr2,
                    x.first->getRoadPhysical()->getStart() + offl2});
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
    polygon.setFillColor(sf::Color(0, 0, 0));
    target.draw(polygon);
  }
  for(const auto& x : startends){
    drawCrosswalk(target, states, x.first, x.second);
  }
}

void IntersectionPhysical::drawCrosswalk(sf::RenderTarget& target,
                                         sf::RenderStates states,
                                         sf::Vector2f start,
                                         sf::Vector2f end) const {
  sf::Vector2f delta = end - start;
  float len = delta.length();
  if (len == 0.f) {
    return;
  }
  sf::Vector2f norm{-delta.y, delta.x};
  norm = norm.normalized();
  float thickness = 4.f;
  float width = 2.f;
  norm = norm * thickness;

  sf::VertexArray v(sf::PrimitiveType::Triangles);

  float curr = 0.f;
  bool highlighed = true;
  while (curr < delta.length()) {
    if (highlighed) {
      sf::Vector2f begin = start + delta.normalized() * curr;
      sf::Vector2f end = start + delta.normalized() * (curr + width);

      v.append(sf::Vertex{begin - norm});
      v.append(sf::Vertex{begin + norm});
      v.append(sf::Vertex{end - norm});

      v.append(sf::Vertex{begin + norm});
      v.append(sf::Vertex{end - norm});
      v.append(sf::Vertex{end + norm});
    }
    curr += width;
    highlighed = !highlighed;
  }
  target.draw(v, states);
}
