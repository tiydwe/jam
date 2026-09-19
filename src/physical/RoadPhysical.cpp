#include "RoadPhysical.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "CarPhysical.h"
#include "Intersection.h"
#include "Lane.h"
#include "Layout.h"
#include "utility.h"

RoadAsset::RoadAsset(std::string filename) {
  this->filename = filename;
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error opening file!" << std::endl;
    return;
  }
  std::string tmp;
  std::getline(file, roadName);
  std::getline(file, tmp);
  {
    std::stringstream ss(tmp);
    double val;
    while (ss >> val) {
      rightCenterOffset.push_back(val);
    }
  }
  std::getline(file, tmp);
  {
    std::stringstream ss(tmp);
    double val;
    while (ss >> val) {
      leftCenterOffset.push_back(val);
    }
  }
  std::getline(file, texturePath);
  std::getline(file, tmp);
  {
    std::stringstream ss(tmp);
    ss >> speedLimit;
  }
  file.close();
}

RoadPhysical::RoadPhysical(size_t id, std::unique_ptr<Road> road,
                           std::unique_ptr<Road> roadb, RoadAsset roadData,
                           sf::Vector2<float> start, sf::Vector2<float> end,
                           bool locked)
    : _id(id),
      _road(std::move(road)),
      _roadb(std::move(roadb)),
      _roadData(roadData),
      _texture(roadData.texturePath),
      _base(_texture, sf::IntRect(sf::Vector2i(start), sf::Vector2i(end))),
      _start(start),
      _end(end),
      _locked(locked) {
  if (!_texture.loadFromFile(roadData.texturePath)) {
    utility::logWarn(
        "RoadPhysical1Way::RoadPhysical1Way - texture path not found.");
  }
  _texture.setRepeated(true);
  _texture.setSmooth(true);
  _base.setTexture(_texture);
  _base.setOrigin({0.f, _texture.getSize().y / 2.f});
  _base.setTextureRect(sf::IntRect(
      {0, 0},
      {(int)sf::Vector2f(_end - _start).length(), (int)_texture.getSize().y}));
  // utility::log("angle: " +
  // std::to_string(sf::Vector2f(_end-_start).angle().asDegrees()));
  _base.setRotation(sf::Vector2f(_end - _start).angle());
  _base.setPosition(sf::Vector2f(_start));
}

RoadPhysical::~RoadPhysical() {}

sf::Vector2f RoadPhysical::getPhysicalPosition(size_t roadid, size_t laneid,
                                               double trueDistance,
                                               CarPhysical* cp,
                                               RoadPhysical* lastRoad,
                                               const Lane* lastLane) const {
  const Road* r = nullptr;
  int mdf = 0;
  const std::vector<int>* offset;
  if (_road->getID() == roadid) {
    r = _road.get();
    mdf = 1;
    offset = &_roadData.rightCenterOffset;
  }
  if (_roadb->getID() == roadid) {
    r = _roadb.get();
    mdf = -1;
    offset = &_roadData.leftCenterOffset;
  }
  if (r == nullptr) {
    utility::logErr("RoadPhysical::getPhysicalPosition - roadid not found");
    utility::exit();
  }
  int lane = r->getLanePosFromCenter(laneid);
  if (lane != -1) {
    double trueBegin = utility::Constants::INTERSECTION_TRANSITION_LENGTH;
    Lane* lanetrue = r->getLaneByID(laneid);
    if (_roadb->getID() == roadid) {
    }
    float percent = (trueDistance) / (lanetrue->getLength());
    if (_roadb->getID() == roadid) {
      percent = 1 - percent;
    }
    auto of = offset->at(lane);
    sf::Vector2f start = _start;
    sf::Vector2f end = _end;
    sf::Vector2f delta = _end - _start;
    delta *= (isRHSRoad(roadid) ? 1.f : -1.f);

    // Right to direction car is traveling
    sf::Vector2f startOffsetNorm{-delta.y, delta.x};
    startOffsetNorm = startOffsetNorm.normalized();
    sf::Vector2f endOffsetNorm{-delta.y, delta.x};
    endOffsetNorm = endOffsetNorm.normalized();
    if (!isRHSRoad(roadid)) {
      std::swap(startOffsetNorm, endOffsetNorm);
    }

    double startOffset = offset->at(lane);
    double endOffset = offset->at(lane);

    if (trueDistance < trueBegin && lastRoad != nullptr &&
        lastLane != nullptr) {
      percent = trueDistance / trueBegin;
      Road* last = lastRoad->isRHSRoad(lastLane->getRoad())
                       ? lastRoad->getRoadR()
                       : lastRoad->getRoadL();

      // inside of start intersection
      if (isRHSRoad(roadid)) {
        end = _start;
      } else {
        end = _end;
      }

      if (lastRoad->isRHSRoad(lastLane->getRoad())) {
        start = lastRoad->getEnd();
        startOffsetNorm = (lastRoad->getEnd() - lastRoad->getStart());
        startOffsetNorm =
            sf::Vector2f(-startOffsetNorm.y, startOffsetNorm.x).normalized();
        startOffset = lastRoad->getRoadAsset()->rightCenterOffset.at(
            last->getLanePosFromCenter(lastLane->getID()));
      } else {
        start = lastRoad->getStart();
        startOffsetNorm = (lastRoad->getStart() - lastRoad->getEnd());
        startOffsetNorm =
            sf::Vector2f(-startOffsetNorm.y, startOffsetNorm.x).normalized();
        startOffset = lastRoad->getRoadAsset()->leftCenterOffset.at(
            last->getLanePosFromCenter(lastLane->getID()));
      }
    }
    sf::Vector2f trueStart =
        start + static_cast<float>(startOffset) * startOffsetNorm;
    sf::Vector2f trueEnd = end + static_cast<float>(endOffset) * endOffsetNorm;
    return trueStart + percent * (trueEnd - trueStart);
  }
  utility::logErr("RoadPhysical::getPhysicalPosition - laneid not found");
  utility::exit();
  return {};
}

sf::Vector2f RoadPhysical::getOffestVectorR() const {
  sf::Vector2f delta = _end - _start;
  sf::Vector2f offsetNorm = {-delta.y, delta.x};
  offsetNorm = offsetNorm.normalized();
  return offsetNorm * getHitbox().getSize().y / 2.f;
}

sf::Vector2f RoadPhysical::getOffestVectorL() const {
  sf::Vector2f delta = _start - _end;
  sf::Vector2f offsetNorm = {-delta.y, delta.x};
  offsetNorm = offsetNorm.normalized();
  return offsetNorm * getHitbox().getSize().y / 2.f;
}

sf::RectangleShape RoadPhysical::getHitbox() const {
  sf::RectangleShape rect;
  sf::FloatRect localBounds = _base.getLocalBounds();

  rect.setSize({localBounds.size.x, localBounds.size.y});
  rect.setOrigin(_base.getOrigin());
  rect.setPosition(_base.getPosition());
  rect.setRotation(_base.getRotation());
  rect.setScale(_base.getScale());

  return rect;
}

void RoadPhysical::draw(sf::RenderTarget& target,
                        sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_base);
}
