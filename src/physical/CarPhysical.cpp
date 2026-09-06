#include "CarPhysical.h"

#include "Game.h"
#include "Simulation.h"
#include "utility.h"
#include "Layout.h"

CarPhysical::CarPhysical(Simulation* game, std::unique_ptr<Car> car,
                         RoadPhysical* rp, std::string texturePath)
    : _game(game),
      _car(std::move(car)),
      _road(rp),
      _texture(texturePath),
      _base(_texture) {
  if (!_texture.loadFromFile(texturePath)) {
    utility::logWarn("CarPhysical::CarPhysical - texture path not found.");
  }
  _base.setTexture(_texture);
  _base.setOrigin({_texture.getSize().x / 2, _texture.getSize().y / 2});
}

void CarPhysical::move(double dt) {
  _car->move(dt);
  _road = _game->getLayout()->getPhysicalRoadFromInternalID(
      _car->getParentSim()->getRoad(_car->getCurrRoad())->getID());
  _base.setPosition(_road->getPhysicalPosition(
      _car->getCurrRoad(), _car->getCurrLane(), _car->getCurrDistFrac()));
}

void CarPhysical::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_base);
}
