#pragma once

#include "Road.h"
#include "SFML/Graphics.hpp"
#include <vector>

// preset road data
class RoadAsset{
  public:
  RoadAsset(std::string filename);

  std::string roadName;
  std::vector<int> rightCenterOffset;
  std::vector<int> leftCenterOffset;
  std::string texturePath;
};

/// @brief Handles physical aspects of a two road, such as position, drawing, etc.
/// Does not handle any simulation road logic.
class RoadPhysical : public sf::Drawable, sf::Transformable {
 public:
  RoadPhysical(Road* road, Road* roadb, RoadAsset roadData, sf::Vector2<int> start,
                   sf::Vector2<int> end);

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  sf::Vector2f getPhysicalPosition(size_t roadid, size_t laneid, double percentDistance) const;

  sf::Vector2<int> getStart() const {return _start;}
  sf::Vector2<int> getEnd() const {return _end;}


 private:
  // on right side of road from start to end
  const Road* _road = nullptr;
  // on left side from start to end
  const Road* _roadb = nullptr;
  RoadAsset _roadData;
  sf::Texture _texture;
  sf::Sprite _base;

  sf::Vector2<int> _start;
  sf::Vector2<int> _end;
};