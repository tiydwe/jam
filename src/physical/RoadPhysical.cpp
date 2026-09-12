#include "RoadPhysical.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "CarPhysical.h"
#include "Lane.h"
#include "utility.h"

RoadAsset::RoadAsset(std::string filename) {
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
                           sf::Vector2<float> start, sf::Vector2<float> end)
    : _id(id),
      _road(std::move(road)),
      _roadb(std::move(roadb)),
      _roadData(roadData),
      _texture(roadData.texturePath),
      _base(_texture, sf::IntRect(sf::Vector2i(start), sf::Vector2i(end))),
      _start(start),
      _end(end) {
  if (!_texture.loadFromFile(roadData.texturePath)) {
    utility::logWarn(
        "RoadPhysical1Way::RoadPhysical1Way - texture path not found.");
  }
  _texture.setRepeated(true);
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
    double percent =
        (trueDistance) / (lanetrue->getLength());
    if (_roadb->getID() == roadid) {
      percent = 1-percent;
    }
    auto of = offset->at(lane);
    sf::Vector2f start = _start;
    sf::Vector2f end = _end;
    if (trueDistance < trueBegin && lastRoad != nullptr && lastLane != nullptr) {
      // inside of start intersection
      percent = trueDistance/trueBegin;
      if (lastRoad->getRoadL()->getLaneByID(lastLane->getID()) != nullptr) {
        of = lastRoad->getRoadAsset()->rightCenterOffset.at(
            lastRoad->getRoadL()->getLanePosFromCenter(lastLane->getID()));
        start = lastRoad->getStart();
      } else if (lastRoad->getRoadR()->getLaneByID(lastLane->getID()) !=
                 nullptr) {
        of = lastRoad->getRoadAsset()->rightCenterOffset.at(
            lastRoad->getRoadR()->getLanePosFromCenter(lastLane->getID()));
        start = lastRoad->getEnd();
      }
      if(_road->getID() == roadid){
        end = _start;
      }
      else{
        end = _end;
      }
    }

    sf::Transform tr;
    tr.rotate(start == end ? sf::radians(0) : sf::Vector2f(end - start).angle());
    sf::Transform tr2;
    tr2.translate(sf::Vector2f(start));
    // utility::log(std::to_string(percentDistnace));
    auto preT =
        sf::Vector2f(percent * sf::Vector2f(end - start).length(), of * mdf);
    auto res = (tr2 * tr).transformPoint(preT);
    return res;
  }
  utility::logErr("RoadPhysical::getPhysicalPosition - laneid not found");
  utility::exit();
  return {};
}

void RoadPhysical::draw(sf::RenderTarget& target,
                        sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_base);
}
