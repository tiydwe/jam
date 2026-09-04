#include "RoadPhysical.h"

#include <fstream>
#include <iostream>
#include <sstream>

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
  file.close();
}

RoadPhysical::RoadPhysical(Road* road, Road* roadb, RoadAsset roadData,
                           sf::Vector2<float> start, sf::Vector2<float> end)
    : _road(road),
      _roadb(roadb),
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
                                               double percentDistnace) const {
  const Road* r = nullptr;
  int mdf = 0;
  const std::vector<int>* offset;
  if (_road->getID() == roadid) {
    r = _road;
    mdf = 1;
    offset = &_roadData.rightCenterOffset;
  }
  if (_roadb->getID() == roadid) {
    r = _roadb;
    mdf = -1;
    offset = &_roadData.leftCenterOffset;
  }
  if (r == nullptr) {
    utility::logErr("RoadPhysical::getPhysicalPosition - roadid not found");
    utility::exit();
  }
  int lane = r->getLanePosFromCenter(laneid);
  if (lane != -1) {
    sf::Transform tr;
    tr.rotate(sf::Vector2f(_end - _start).angle());
    sf::Transform tr2;
    tr2.translate(sf::Vector2f(_start));
    auto of = offset->at(lane);
    //utility::log(std::to_string(percentDistnace));
    auto preT = sf::Vector2f(
        percentDistnace * sf::Vector2f(_end - _start).length(), of * mdf);
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
