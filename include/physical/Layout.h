#pragma once

#include "RoadPhysical.h"
#include "IntersectionPhysical.h"
#include <string>
#include "Simulation.h"

class Layout{
  public:
  Layout() = default;
  Layout(std::string filepath, Simulation* s);
  ~Layout();

  RoadPhysical* getFromInternalRoadID(size_t internalRoadID);
  
  Simulation* _s;
  // owns these
  std::map<size_t, RoadPhysical*> _physicalRoads;
  std::map<size_t, IntersectionPhysical*> _physicalIntersections;
};