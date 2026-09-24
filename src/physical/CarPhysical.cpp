#include "CarPhysical.h"

#include "Game.h"
#include "Layout.h"
#include "Road.h"
#include "Simulation.h"
#include "utility.h"

CarPhysical::CarPhysical(Simulation* game, std::unique_ptr<Car> car,
                         RoadPhysical* rp, std::string texturePath)
    : _game(game),
      _car(std::move(car)),
      _road(rp),
      _texture(texturePath),
      _base(_texture),
      _noRouteIcon(utility::Constants::noRouteIcon) {
  if (!_texture.loadFromFile(texturePath)) {
    utility::logWarn("CarPhysical::CarPhysical - texture path not found.");
  }
  float nriscale = 0.8f;
  _noRouteIcon.setScale({nriscale, nriscale});
  _noRouteIcon.setOrigin(_noRouteIcon.getLocalBounds().size / 2.f);
  _base.setTexture(_texture);
  _base.setOrigin({_texture.getSize().x / 2, _texture.getSize().y / 2});
}

void CarPhysical::move(double dt) {
  _car->move(dt);
  _road = _game->getLayout()->getPhysicalRoadFromInternalID(
      _car->getParentSim()->getLayout()->getRoad(_car->getCurrRoad())->getID());
  auto ans = _road->getPhysicalPosition(
      _car->getCurrRoad(), _car->getCurrLane(), _car->getCurrDist(), this,
      _car->getLastRoad() == nullptr
          ? nullptr
          : _game->getLayout()->getPhysicalRoadFromInternalID(
                _car->getLastRoad()->getID()),
      _car->getLastLane());
  _base.setPosition(ans.first);
  _base.setRotation(ans.second);
  sf::Vector2f positionNoRoute{_base.getPosition().x,
                               _base.getPosition().y - 40};
  _noRouteIcon.setPosition(positionNoRoute);
}

void CarPhysical::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  states.transform *= getTransform();
  if (_car->getStatus() != carStatus::ARRIVED) {
    // pro-tip: this one weird trick makes your cpu cry
    auto cpy = _base;
    if (_isHovering) {
      cpy.setColor(sf::Color(150, 150, 150));
    } else {
      cpy.setColor(sf::Color(255, 255, 255));
    }
    target.draw(cpy);
  }
  if (_isHovering) {
    // show destination
    auto destPos =
        _game->getLayout()
            ->getPhysicalRoadFromInternalID(_car->getParentSim()
                                                ->getLayout()
                                                ->getRoad(_car->getDestRoad())
                                                ->getID())
            ->getPhysicalPosition(
                _car->getDestRoad(), _car->getDestLane(), _car->getDestDist(),
                this,
                _car->getLastRoad() == nullptr
                    ? nullptr
                    : nullptr /*just hope that it doesnt go over an edge*/,
                _car->getLastLane())
            .first;
    sf::CircleShape destVis;
    destVis.setRadius(8.f);
    destVis.setOrigin(destVis.getLocalBounds().size / 2.f);
    destVis.setPosition(destPos);
    destVis.setFillColor(sf::Color(34, 189, 28));
    target.draw(destVis, states);
  }
  if (_car->getStatus() == carStatus::NO_ROUTE) {
    target.draw(_noRouteIcon, states);
  }
}
