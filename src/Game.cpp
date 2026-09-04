#include "Game.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "utility.h"

Layout::Layout(std::string filepath, Simulation* s) : _s(s) {
  std::ifstream file(filepath);

  if (!file.is_open()) {
    utility::logErr("Error opening layout file! " + filepath);
    return;
  }
  std::string tmp;

  while (std::getline(file, tmp)) {
    std::stringstream ss(tmp);
    std::string type;
    ss >> type;
    if (type == "I") {
      // intersection
      sf::Vector2f pos;
      size_t id;
      ss >> id >> pos.x >> pos.y;
      Intersection* i = new Intersection(*_s);
      IntersectionPhysical* ip = new IntersectionPhysical(i, pos);
      _physicalIntersections.try_emplace(id, ip);
      _s->addIntersection(i);
    } else if (type == "#") {
      continue;
    } else {
      break;
    }
  }
  while (std::getline(file, tmp)) {
    std::stringstream ss(tmp);
    std::string type;
    ss >> type;
    if (type == "R2") {
      // road
      std::string datapath;
      sf::Vector2f startpos;
      sf::Vector2f endpos;
      size_t startid;
      size_t endid;
      size_t id;
      size_t laneslhs, lanesrhs;
      double speedlimit;
      ss >> id >> datapath >> startid >> endid >> laneslhs >> lanesrhs >>
          speedlimit;
      startpos = _physicalIntersections.at(startid)->getPos();
      endpos = _physicalIntersections.at(endid)->getPos();
      double angle = (endpos - startpos).angle().asRadians();
      double length = (endpos - startpos).length();

      Road* r_rhs =
          new Road(*_s, speedlimit,
                   _physicalIntersections.at(endid)->getInternalID(), angle);
      Road* r_lhs =
          new Road(*_s, speedlimit,
                   _physicalIntersections.at(startid)->getInternalID(), angle);
#ifdef DEBUG
      utility::log("Added road " + std::to_string(r_rhs->getID()));
      utility::log("Added road " + std::to_string(r_lhs->getID()));
#endif
      for (int i = 0; i < lanesrhs; ++i) {
        Lane* l = new Lane(*_s, length, r_rhs->getID());
        r_rhs->addLane(l->getID());
        _s->addLane(l);
      }
      for (int i = 0; i < laneslhs; ++i) {
        Lane* l = new Lane(*_s, length, r_lhs->getID());
        r_lhs->addLane(l->getID());
        _s->addLane(l);
      }
      _s->addRoad(r_rhs);
      _s->addRoad(r_lhs);
      auto& r_rhs2 = _s->getRoad(r_rhs->getID());
      auto& r_lhs2 = _s->getRoad(r_lhs->getID());
      _s->getIntersection(_physicalIntersections.at(endid)->getInternalID())
          .addIngoing(r_rhs2.getID());
      _s->getIntersection(_physicalIntersections.at(endid)->getInternalID())
          .addOutgoing(r_lhs2.getID());
      _s->getIntersection(_physicalIntersections.at(endid)->getInternalID())
          .getTrafficLight()
          .reSchedule(GREEN_PHASE_TIME_DEFAULT, YELLOW_PHASE_TIME_DEFAULT);
      _s->getIntersection(_physicalIntersections.at(startid)->getInternalID())
          .addOutgoing(r_rhs2.getID());
      _s->getIntersection(_physicalIntersections.at(startid)->getInternalID())
          .addIngoing(r_lhs2.getID());
      _s->getIntersection(_physicalIntersections.at(startid)->getInternalID())
          .getTrafficLight()
          .reSchedule(GREEN_PHASE_TIME_DEFAULT, YELLOW_PHASE_TIME_DEFAULT);

      RoadPhysical* rp = new RoadPhysical(&r_rhs2, &r_lhs2, RoadAsset(datapath),
                                          startpos, endpos);

      _physicalRoads.try_emplace(id, rp);

    } else if (type == "#") {
      continue;
    } else {
      break;
    }
  }
}

Layout::~Layout() {}

RoadPhysical* Layout::getFromInternalRoadID(size_t internalRoadID) {
  for(auto& x : _physicalRoads){
    if(x.second->getInternalIDL() == internalRoadID || x.second->getInternalIDR() == internalRoadID){
      return x.second;
    }
  }
  utility::logWarn("Layout::getFromInternalRoadID - internalRoadID not found!");
  return nullptr;
}

CarPhysical* createPhysicalFromDataFile(Game* game, Car* car, RoadPhysical* roadPhysical,
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

Game::Game() : _s() {}

Game::Game(std::string filepath) {
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
  for (auto& x : _l._physicalRoads) {
    target.draw(*x.second, states);
  }
  for (auto& x : _l._physicalIntersections) {
    target.draw(*x.second, states);
  }
  for (auto& x : _physicalCars) {
    target.draw(*x.second, states);
  }
}
