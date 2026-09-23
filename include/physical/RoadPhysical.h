#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "Lane.h"
#include "Road.h"
#include "SFML/Graphics.hpp"

class CarPhysical;

// preset road data
class RoadAsset {
 public:
  RoadAsset(std::string filename);

  std::string roadName;
  std::vector<int> rightCenterOffset;
  std::vector<int> leftCenterOffset;
  std::string texturePath;
  double speedLimit;
  std::string filename;
};

/// @brief Handles physical aspects of a two road, such as position, drawing,
/// etc. Does not handle any simulation road logic.
class RoadPhysical : public sf::Drawable, sf::Transformable {
 public:
  RoadPhysical(const RoadPhysical&) = delete;
  RoadPhysical& operator=(const RoadPhysical&) = delete;

  RoadPhysical(RoadPhysical&&) = delete;
  RoadPhysical& operator=(RoadPhysical&&) = delete;
  RoadPhysical(size_t id, std::unique_ptr<Road> road,
               std::unique_ptr<Road> roadb, RoadAsset roadData,
               sf::Vector2<float> start, sf::Vector2<float> end, bool locked = true);
  
  // auto deletes intersections
  ~RoadPhysical();

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  std::pair<sf::Vector2f, sf::Angle> getPhysicalPosition(size_t roadid, size_t laneid,
                                   double trueDistance, const CarPhysical* cp,
                                   RoadPhysical* lastRoad,
                                   const Lane* lastLane) const;

  sf::Vector2<float> getStart() const { return _start; }
  sf::Vector2<float> getEnd() const { return _end; }
  size_t getInternalIDL() const { return _roadb->getID(); }
  size_t getInternalIDR() const { return _road->getID(); }
  Road* getRoadR() const { return _road.get(); }
  Road* getRoadL() const { return _roadb.get(); }
  size_t getID() const { return _id; }
  double getLength() const { return (_start - _end).length(); }
  const RoadAsset* getRoadAsset() const {return &_roadData;}
  bool isRHSRoad(size_t id) const {return _road->getID() == id;}
  // Returns a vector from start to edge at start
  sf::Vector2f getOffestVectorR() const;
  sf::Vector2f getOffestVectorL() const;
  bool isLocked() const {return _locked;}
  sf::RectangleShape getHitbox() const;

 private:
  size_t _id;
  // on right side of road from start to end
  std::unique_ptr<Road> _road;
  // on left side from start to end
  std::unique_ptr<Road> _roadb;
  RoadAsset _roadData;
  sf::Texture _texture;
  sf::Sprite _base;

  sf::Vector2<float> _start;
  sf::Vector2<float> _end;

  bool _locked;
};