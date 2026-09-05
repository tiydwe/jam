#include "Game.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "utility.h"

CarPhysical* createPhysicalFromDataFile(Game* game, Car* car,
                                        RoadPhysical* roadPhysical,
                                        std::string dataFilename) {
  std::ifstream file(dataFilename);
  if (!file.is_open()) {
    utility::logErr("Error opening data filename " + dataFilename);
    return nullptr;
  }
  std::string tmp;
  std::getline(file, tmp);
  std::getline(file, tmp);
  return new CarPhysical(game, car, roadPhysical, tmp);
}

Game::Game(sf::RenderWindow& window) : _s(), _ew(&_l, this, window.getSize()) {}

Game::Game(std::string filepath, sf::RenderWindow& window)
    : _s(), _l(getLayoutPath(filepath), &_s), _ew(&_l, this, window.getSize()) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    utility::logErr("Error opening game file! " + filepath);
    return;
  }
  std::string tmp;
  std::getline(file, tmp);
  _l = Layout(tmp, &_s);
  while (std::getline(file, tmp)) {
    std::stringstream ss(tmp);
    std::string type;
    ss >> type;
    if (type == "C") {
      size_t start, end;
      char lrs, lre;
      double initDist, finDist, maxa;
      std::string datafilename;
      ss >> datafilename >> start >> lrs >> end >> lre >> initDist >> finDist >>
          maxa;
      Car* cp = nullptr;
      if (lrs == 'l') {
        cp = new Car(_s, _l._physicalRoads.at(start)->getInternalIDL(),
                     _s.getRoad(_l._physicalRoads.at(start)->getInternalIDL())
                         .getEdgeLane(),
                     initDist, maxa);
      } else {
        cp = new Car(_s, _l._physicalRoads.at(start)->getInternalIDR(),
                     _s.getRoad(_l._physicalRoads.at(start)->getInternalIDR())
                         .getEdgeLane(),
                     initDist, maxa);
      }
      if (lre == 'l') {
        cp->setDestination(
            {_l._physicalRoads.at(end)->getInternalIDL(), finDist});
      } else {
        cp->setDestination(
            {_l._physicalRoads.at(end)->getInternalIDR(), finDist});
      }
      CarPhysical* cpp = nullptr;
      cpp = createPhysicalFromDataFile(this, cp, _l._physicalRoads.at(start),
                                       datafilename);
      _physicalCars[cp->getID()] = cpp;
      cp->recalcRoute();
      _s.addCar(cp);
    } else if (type == "#") {
      continue;
    } else {
      break;
    }
  }
  _ew = EditorWindow(&_l, this, window.getSize());
}

Game::~Game() {
  for (auto& x : _physicalCars) {
    delete x.second;
  }
  _physicalCars.clear();
}

void Game::step(double dt) {
  _s.step(dt);
  for (auto& x : _physicalCars) {
    x.second->update();
  }
}

void Game::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  _ew.draw(target, states);
}

std::string Game::getLayoutPath(const std::string& filepath) {
  std::ifstream file(filepath);

  if (!file.is_open()) {
    utility::logErr("Error opening game file! " + filepath);
    return {};
  }

  std::string tmp;
  std::getline(file, tmp);
  return tmp;
}