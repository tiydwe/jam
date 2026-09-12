#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>

#include "Car.h"
#include "IntersectionPhysical.h"
#include "Lane.h"
#include "Road.h"
#include "RoadPhysical.h"
#include "utility.h"

#include <SFML/Graphics.hpp>

class Layout : public sf::Drawable, sf::Transformable {
 public:
  Layout(std::string filename);
  ~Layout();

  IntersectionPhysical* createIntersection(sf::Vector2f& position);
  RoadPhysical* createRoad(IntersectionPhysical& start, IntersectionPhysical& end, std::string datapath);

  // returns nullptr if no intersections
  std::pair<IntersectionPhysical*, double> findClosest(sf::Vector2f& pos);

  Road* getRoad(size_t id) const { return _roads.at(id); }
  RoadPhysical* getPhysicalRoad(size_t id) const {return _physicalRoads.at(id).get();}
  Lane* getLane(size_t id) const { return _lanes.at(id).get(); }
  std::map<size_t, Lane*> getLanes() const;
  IntersectionPhysical* getIntersection(size_t id) const {
    return _physicalIntersections.at(id).get();
  }
  std::map<size_t, IntersectionPhysical*> getPhysicalIntersections() const;
  Intersection* getIntersectionFromInternalID(size_t id)const{ return _intersections.at(id); }
  RoadPhysical* getPhysicalRoadFromInternalID(size_t id) const;
  IntersectionPhysical* getBeginIntersectionFromInternalRoadID(size_t id) const;
  Road* getRoadBetweenTwoIntersectionsFromInternalID(size_t begin, size_t end) const;
  

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 private:
  std::map<size_t, std::unique_ptr<Lane>> _lanes;
  std::map<size_t, Road*> _roads;
  std::map<size_t, Intersection*> _intersections;

  // id is NOT the same as logical ids
  std::map<size_t, std::unique_ptr<RoadPhysical>> _physicalRoads;
  std::map<size_t, std::unique_ptr<IntersectionPhysical>>
      _physicalIntersections;
};