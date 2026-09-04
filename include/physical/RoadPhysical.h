#pragma once

#include <vector>

#include "Road.h"
#include "SFML/Graphics.hpp"

// preset road data
class RoadAsset {
 public:
  RoadAsset(std::string filename);

  std::string roadName;
  std::vector<int> rightCenterOffset;
  std::vector<int> leftCenterOffset;
  std::string texturePath;
};

/// @brief Handles physical aspects of a two road, such as position, drawing,
/// etc. Does not handle any simulation road logic.
class RoadPhysical : public sf::Drawable, sf::Transformable {
 public:
  RoadPhysical(const RoadPhysical&) = delete;
  RoadPhysical& operator=(const RoadPhysical&) = delete;

  RoadPhysical(RoadPhysical&&) = delete;
  RoadPhysical& operator=(RoadPhysical&&) = delete;
  RoadPhysical(Road* road, Road* roadb, RoadAsset roadData,
               sf::Vector2<float> start, sf::Vector2<float> end);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  sf::Vector2f getPhysicalPosition(size_t roadid, size_t laneid,
                                   double percentDistance) const;

  sf::Vector2<float> getStart() const { return _start; }
  sf::Vector2<float> getEnd() const { return _end; }
  size_t getInternalIDL() const { return _roadb->getID(); }
  size_t getInternalIDR() const { return _road->getID(); }

 private:
  // on right side of road from start to end
  const Road* _road = nullptr;
  // on left side from start to end
  const Road* _roadb = nullptr;
  RoadAsset _roadData;
  sf::Texture _texture;
  sf::Sprite _base;

  sf::Vector2<float> _start;
  sf::Vector2<float> _end;
};