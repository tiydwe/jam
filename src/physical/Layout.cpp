#include "Layout.h"

#include <filesystem>
#include <fstream>
#include <istream>
#include <limits>
#include <sstream>

Layout::Layout(std::filesystem::path filepath, std::filesystem::path carpath,
               std::filesystem::path victorypath,
               std::filesystem::path progresspath)
    : _carfpath(carpath),
      _victoryfpath(victorypath),
      _progressfpath(progresspath) {
  std::ifstream file(filepath);

  if (!file.is_open()) {
    utility::logErr("Error opening layout file! " + filepath.string());
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
      char locked;
      ss >> id >> datapath >> startid >> endid >> laneslhs >> lanesrhs >>
          locked;
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
      auto correctionVector =
          delta.normalized() * utility::Constants::INTERSESCTION_SIZE;
      std::unique_ptr<RoadPhysical> rp = std::make_unique<RoadPhysical>(
          id, std::move(r_rhs), std::move(r_lhs), ra,
          startpos + correctionVector, endpos - correctionVector,
          locked == 'l');
      RoadPhysical* rpp = rp.get();
      _roads.try_emplace(rp->getInternalIDR(), rp->getRoadR());
      _roads.try_emplace(rp->getInternalIDL(), rp->getRoadL());
      _physicalRoads.try_emplace(id, std::move(rp));

      if (rpp->getRoadR()->getNumLanes() > 0) {
        _physicalIntersections.at(endid)->getIntersection()->addIngoing(
            r_rhsID);
        _physicalIntersections.at(startid)->getIntersection()->addOutgoing(
            r_rhsID);
      }

      if (rpp->getRoadL()->getNumLanes() > 0) {
        _physicalIntersections.at(endid)->getIntersection()->addOutgoing(
            r_lhsID);
        _physicalIntersections.at(startid)->getIntersection()->addIngoing(
            r_lhsID);
      }
    } else if (type == "#") {
      continue;
    } else {
      break;
    }
  }
}

Layout::~Layout() {}

void Layout::saveToFile(std::filesystem::path fpath) {
  std::filesystem::path saveDir =
      std::filesystem::absolute(std::filesystem::weakly_canonical(fpath));

  // LAYOUT FILE
  auto layoutPath = saveDir / "layout.dat";
  std::ofstream layoutFile(layoutPath);
  if (!layoutFile) {
    utility::logErr("Layout::saveToFile - error opening file " +
                    layoutPath.string());
    return;
  }
  for (const auto& x : _physicalIntersections) {
    layoutFile << "I " << x.first << " " << x.second->getPos().x << " "
               << x.second->getPos().y << "\n";
  }
  layoutFile << "SW\n";
  for (const auto& x : _physicalRoads) {
    auto l = x.second->getRoadL();
    auto r = x.second->getRoadR();
    layoutFile
        << "R2 " << x.first << " " << x.second->getRoadAsset()->filename << " "
        << this->getIntersectionPhysicalFromInternalID(l->getEndIntersection())
               ->getID()
        << " "
        << this->getIntersectionPhysicalFromInternalID(r->getEndIntersection())
               ->getID()
        << " " << x.second->getRoadAsset()->leftCenterOffset.size() << " "
        << x.second->getRoadAsset()->rightCenterOffset.size() << " "
        << (x.second->isLocked() ? 'l' : 'u') << "\n";
  }

  // MAIN
  std::filesystem::path mainPath = saveDir / "main.dat";
  std::ofstream mainFile(mainPath);
  if (!mainFile) {
    utility::logErr("Layout::saveToFile - error opening file " +
                    mainPath.string());
    return;
  }
  mainFile << layoutPath.string() << "\n"
           << this->_carfpath.string() << "\n"
           << this->_victoryfpath.string() << "\n"
           << this->_progressfpath.string();
}

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
                                 std::string datapath, bool locked) {
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
  auto correctionVector =
      delta.normalized() * utility::Constants::INTERSESCTION_SIZE;
  std::unique_ptr<RoadPhysical> rp = std::make_unique<RoadPhysical>(
      utility::getNewPhysicalID(), std::move(r_rhs), std::move(r_lhs), ra,
      startpos + correctionVector, endpos - correctionVector, locked);
  RoadPhysical* rpp = rp.get();
  _roads.try_emplace(rp->getInternalIDR(), rp->getRoadR());
  _roads.try_emplace(rp->getInternalIDL(), rp->getRoadL());
  _physicalRoads.try_emplace(rp->getID(), std::move(rp));

  // utility::log(std::to_string(endid));
  if (rpp->getRoadR()->getNumLanes() > 0) {
    _physicalIntersections.at(endid)->getIntersection()->addIngoing(r_rhsID);
    _physicalIntersections.at(startid)->getIntersection()->addOutgoing(r_rhsID);
  }

  if (rpp->getRoadL()->getNumLanes() > 0) {
    _physicalIntersections.at(endid)->getIntersection()->addOutgoing(r_lhsID);
    _physicalIntersections.at(startid)->getIntersection()->addIngoing(r_lhsID);
  }

  _physicalIntersections.at(endid)
      ->getIntersection()
      ->getTrafficLight()
      ->reSchedule(utility::Constants::GREEN_PHASE_TIME_DEFAULT,
                   utility::Constants::YELLOW_PHASE_TIME_DEFAULT);
  _physicalIntersections.at(startid)
      ->getIntersection()
      ->getTrafficLight()
      ->reSchedule(utility::Constants::GREEN_PHASE_TIME_DEFAULT,
                   utility::Constants::YELLOW_PHASE_TIME_DEFAULT);
  return rpp;
}

std::pair<bool, sf::RectangleShape> Layout::checkHitbox(
    sf::RectangleShape hitbox) {
  for (const auto& x : _physicalRoads) {
    if (utility::rectanglesIntersect(x.second->getHitbox(), hitbox)) {
      return {true, x.second->getHitbox()};
    }
  }
  return {false, sf::RectangleShape{}};
}

void Layout::removeRoad(size_t id) {
  std::unique_ptr<RoadPhysical> rp = std::move(_physicalRoads.at(id));
  _physicalRoads.erase(id);
  Road* roadr = rp->getRoadR();
  Road* roadl = rp->getRoadL();

  Intersection* startIntersection =
      this->getIntersectionFromInternalID(roadl->getEndIntersection());
  Intersection* endIntersection =
      this->getIntersectionFromInternalID(roadr->getEndIntersection());
  startIntersection->removeOutgoing(roadr->getID());
  startIntersection->removeIngoing(roadl->getID());
  startIntersection->getTrafficLight()->reSchedule(
      utility::Constants::GREEN_PHASE_TIME_DEFAULT,
      utility::Constants::YELLOW_PHASE_TIME_DEFAULT);
  endIntersection->removeOutgoing(roadl->getID());
  endIntersection->removeIngoing(roadr->getID());
  endIntersection->getTrafficLight()->reSchedule(
      utility::Constants::GREEN_PHASE_TIME_DEFAULT,
      utility::Constants::YELLOW_PHASE_TIME_DEFAULT);
  if (startIntersection->getRoads().empty()) {
    _physicalIntersections.erase(
        this->getIntersectionPhysicalFromInternalID(roadl->getEndIntersection())
            ->getID());
  }
  if (endIntersection->getRoads().empty()) {
    _physicalIntersections.erase(
        this->getIntersectionPhysicalFromInternalID(roadr->getEndIntersection())
            ->getID());
  }

  for (size_t id : roadr->getLanes()) {
    _lanes.erase(id);
  }
  for (size_t id : roadl->getLanes()) {
    _lanes.erase(id);
  }

  rp.reset();
}

std::pair<IntersectionPhysical*, double> Layout::findClosestIntersection(
    sf::Vector2f pos) {
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

std::pair<RoadPhysical*, double> Layout::findClosestRoadUnlocked(
    sf::Vector2f pos) {
  RoadPhysical* closest = nullptr;
  double bestDist = std::numeric_limits<double>::max();
  for (const auto& x : _physicalRoads) {
    if (x.second->isLocked()) {
      continue;
    }
    auto a = x.second->getStart();
    auto b = x.second->getEnd();
    float t = (pos - a).dot(b - a) / (a - b).lengthSquared();
    double newDist = 0;
    if (t <= 0) {
      newDist = (pos - a).length();
    } else if (t >= 1) {
      newDist = (pos - b).length();
    } else {
      sf::Vector2f pt = a + (b - a) * t;
      newDist = (pt - pos).length();
    }
    if (newDist < bestDist) {
      bestDist = newDist;
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

std::map<size_t, IntersectionPhysical*> Layout::getPhysicalIntersections()
    const {
  std::map<size_t, IntersectionPhysical*> res;
  for (const auto& x : _physicalIntersections) {
    res.try_emplace(x.first, x.second.get());
  }
  return res;
}

IntersectionPhysical* Layout::getIntersectionPhysicalFromInternalID(
    size_t id) const {
  for (const auto& x : _physicalIntersections) {
    if (x.second->getInternalID() == id) {
      return x.second.get();
    }
  }
  return nullptr;
}

RoadPhysical* Layout::getPhysicalRoadFromInternalID(size_t id) const {
  for (auto& x : _physicalRoads) {
    if (x.second->getInternalIDL() == id || x.second->getInternalIDR() == id) {
      return x.second.get();
    }
  }
  utility::logWarn(
      "Layout::getPhysicalRoadFromInternalID - internalRoadID not found!");
  return nullptr;
}

IntersectionPhysical* Layout::getBeginIntersectionFromInternalRoadID(
    size_t id) const {
  for (const auto& x : _physicalIntersections) {
    for (auto o : x.second->getIntersection()->getOutgoings()) {
      if (o == id) {
        return x.second.get();
      }
    }
  }
  return nullptr;
}

Road* Layout::getRoadBetweenTwoIntersectionsFromInternalID(size_t begin,
                                                           size_t end) const {
  for (const auto& x : getIntersectionFromInternalID(begin)->getOutgoings()) {
    if (getRoad(x)->getEndIntersection() == end) {
      return getRoad(x);
    }
  }
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
