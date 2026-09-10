#include "Layout.h"

#include <fstream>
#include <istream>
#include <sstream>
#include <limits>

Layout::Layout(std::string filepath) {
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
      utility::registerPhysicalID(id);
      std::unique_ptr<Intersection> i = std::make_unique<Intersection>(this);
      size_t internalid = i->getID();
      std::unique_ptr<IntersectionPhysical> ip =
          std::make_unique<IntersectionPhysical>(std::move(i), pos, id);
      _physicalIntersections.try_emplace(id, std::move(ip));
      _intersections.try_emplace(
          internalid, _physicalIntersections.at(id)->getIntersection());
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
      ss >> id >> datapath >> startid >> endid >> laneslhs >> lanesrhs;
      auto ra = RoadAsset(datapath);
      double speedlimit = ra.speedLimit;
      utility::registerPhysicalID(id);
      startpos = _physicalIntersections.at(startid)->getPos();
      endpos = _physicalIntersections.at(endid)->getPos();
      double angle = (endpos - startpos).angle().asRadians();
      double length = (endpos - startpos).length();

      std::unique_ptr<Road> r_rhs = std::make_unique<Road>(
          this, speedlimit, _physicalIntersections.at(endid)->getInternalID(),
          angle);
      std::unique_ptr<Road> r_lhs = std::make_unique<Road>(
          this, speedlimit, _physicalIntersections.at(startid)->getInternalID(),
          angle);
#ifdef DEBUG
      utility::log("Added road " + std::to_string(r_rhs->getID()));
      utility::log("Added road " + std::to_string(r_lhs->getID()));
#endif
      for (int i = 0; i < lanesrhs; ++i) {
        std::unique_ptr<Lane> l =
            std::make_unique<Lane>(this, length, r_rhs->getID());
        r_rhs->addLane(l->getID());
        _lanes.try_emplace(l->getID(), std::move(l));
      }
      for (int i = 0; i < laneslhs; ++i) {
        std::unique_ptr<Lane> l =
            std::make_unique<Lane>(this, length, r_lhs->getID());
        r_lhs->addLane(l->getID());
        _lanes.try_emplace(l->getID(), std::move(l));
      }
      size_t r_rhsID = r_rhs->getID();
      size_t r_lhsID = r_lhs->getID();
      auto delta = endpos - startpos;
      auto correctionVector = delta.normalized() * INTERSESCTION_SIZE;
      std::unique_ptr<RoadPhysical> rp = std::make_unique<RoadPhysical>(
          id, std::move(r_rhs), std::move(r_lhs), ra,
          startpos + correctionVector, endpos - correctionVector);
      _roads.try_emplace(rp->getInternalIDR(), rp->getRoadR());
      _roads.try_emplace(rp->getInternalIDL(), rp->getRoadL());
      _physicalRoads.try_emplace(id, std::move(rp));

      _physicalIntersections.at(endid)->getIntersection()->addIngoing(r_rhsID);
      _physicalIntersections.at(endid)->getIntersection()->addOutgoing(r_lhsID);
      _physicalIntersections.at(endid)
          ->getIntersection()
          ->getTrafficLight()
          ->reSchedule(GREEN_PHASE_TIME_DEFAULT, YELLOW_PHASE_TIME_DEFAULT);

      _physicalIntersections.at(startid)->getIntersection()->addOutgoing(
          r_rhsID);
      _physicalIntersections.at(startid)->getIntersection()->addIngoing(
          r_lhsID);
      _physicalIntersections.at(startid)
          ->getIntersection()
          ->getTrafficLight()
          ->reSchedule(GREEN_PHASE_TIME_DEFAULT, YELLOW_PHASE_TIME_DEFAULT);

    } else if (type == "#") {
      continue;
    } else {
      break;
    }
  }
}

Layout::~Layout() {}

IntersectionPhysical* Layout::createIntersection(sf::Vector2f& position) {
  std::unique_ptr<Intersection> i = std::make_unique<Intersection>(this);
  std::unique_ptr<IntersectionPhysical> ip =
      std::make_unique<IntersectionPhysical>(std::move(i), position,
                                             utility::getNewPhysicalID());
  Intersection* intersectionptr = ip->getIntersection();
  size_t id = ip->getID();
  _physicalIntersections.try_emplace(ip->getID(), std::move(ip));
  _intersections.try_emplace(intersectionptr->getID(), intersectionptr);
  return _physicalIntersections.at(id).get();
}

RoadPhysical* Layout::createRoad(IntersectionPhysical& start,
                                 IntersectionPhysical& end,
                                 std::string datapath) {
  auto startpos = start.getPos();
  auto endpos = end.getPos();
  auto startid = start.getID();
  auto endid = end.getID();
  double angle = (endpos - startpos).angle().asRadians();
  double length = (endpos - startpos).length();
  RoadAsset ra(datapath);
  std::unique_ptr<Road> r_rhs =
      std::make_unique<Road>(this, ra.speedLimit, end.getInternalID(), angle);
  std::unique_ptr<Road> r_lhs =
      std::make_unique<Road>(this, ra.speedLimit, start.getInternalID(), angle);

  for (int i = 0; i < ra.rightCenterOffset.size(); ++i) {
    std::unique_ptr<Lane> l =
        std::make_unique<Lane>(this, length, r_rhs->getID());
    r_rhs->addLane(l->getID());
    _lanes.try_emplace(l->getID(), std::move(l));
  }
  for (int i = 0; i < ra.leftCenterOffset.size(); ++i) {
    std::unique_ptr<Lane> l =
        std::make_unique<Lane>(this, length, r_lhs->getID());
    r_lhs->addLane(l->getID());
    _lanes.try_emplace(l->getID(), std::move(l));
  }
  size_t r_rhsID = r_rhs->getID();
  size_t r_lhsID = r_lhs->getID();
  auto delta = endpos - startpos;
  auto correctionVector = delta.normalized() * INTERSESCTION_SIZE;
  std::unique_ptr<RoadPhysical> rp = std::make_unique<RoadPhysical>(
      utility::getNewPhysicalID(), std::move(r_rhs), std::move(r_lhs), ra,
      startpos + correctionVector, endpos - correctionVector);
  RoadPhysical* rpp = rp.get();
  _roads.try_emplace(rp->getInternalIDR(), rp->getRoadR());
  _roads.try_emplace(rp->getInternalIDL(), rp->getRoadL());
  _physicalRoads.try_emplace(rp->getID(), std::move(rp));

  utility::log(std::to_string(endid));
  _physicalIntersections.at(endid)->getIntersection()->addIngoing(r_rhsID);
  _physicalIntersections.at(endid)->getIntersection()->addOutgoing(r_lhsID);
  _physicalIntersections.at(endid)
      ->getIntersection()
      ->getTrafficLight()
      ->reSchedule(GREEN_PHASE_TIME_DEFAULT, YELLOW_PHASE_TIME_DEFAULT);

  _physicalIntersections.at(startid)->getIntersection()->addOutgoing(r_rhsID);
  _physicalIntersections.at(startid)->getIntersection()->addIngoing(r_lhsID);
  _physicalIntersections.at(startid)
      ->getIntersection()
      ->getTrafficLight()
      ->reSchedule(GREEN_PHASE_TIME_DEFAULT, YELLOW_PHASE_TIME_DEFAULT);
  return rpp;
}

std::pair<IntersectionPhysical*, double> Layout::findClosest(sf::Vector2f& pos) {
  IntersectionPhysical* closest = nullptr;
  double bestDist = std::numeric_limits<double>::max();
  for (const auto& x : _physicalIntersections) {
    if ((pos - x.second->getPos()).length() < bestDist) {
      bestDist = (pos - x.second->getPos()).length();
      closest = x.second.get();
    }
  }
  return {closest, bestDist};
}

std::map<size_t, Lane*> Layout::getLanes() const {
  std::map<size_t, Lane*> res;
  for (const auto& x : _lanes) {
    res.try_emplace(x.first, x.second.get());
  }
  return res;
}

RoadPhysical* Layout::getPhysicalRoadFromInternalID(size_t id) {
  for (auto& x : _physicalRoads) {
    if (x.second->getInternalIDL() == id || x.second->getInternalIDR() == id) {
      return x.second.get();
    }
  }
  utility::logWarn(
      "Layout::getPhysicalRoadFromInternalID - internalRoadID not found!");
  return nullptr;
}

void Layout::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  for (const auto& x : _physicalRoads) {
    x.second->draw(target, states);
  }
  for (const auto& x : _physicalIntersections) {
    x.second->draw(target, states);
  }
}
