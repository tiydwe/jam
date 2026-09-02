#include "CarPhysical.h"

#include "utility.h"

CarPhysical::CarPhysical(Car* car, RoadPhysical* rp, std::string texturePath)
    : _car(car), _road(rp), _texture(texturePath), _base(_texture) {
  if (!_texture.loadFromFile(texturePath)) {
    utility::logWarn("CarPhysical::CarPhysical - texture path not found.");
  }
  _base.setTexture(_texture);
  _base.setOrigin({_texture.getSize().x / 2, _texture.getSize().y / 2});
}

void CarPhysical::update() {
  _base.setPosition(_road->getPhysicalPosition(
      _car->getCurrRoad(), _car->getCurrLane(), _car->getCurrDistFrac()));
}

void CarPhysical::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_base);
}
