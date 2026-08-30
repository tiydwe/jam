#include "Road.h"

#include "utility.h"
#include "Simulation.h"

Road::Road() {
  utility::exit("This should never be called (probally something messed up in map!)");
}

Road::Road(Simulation& parent, double speedLimit, size_t endIntersection)
  : _parent(&parent),
    _id(utility::uid()),
    _speed_limit(speedLimit),
    _endIntersection(endIntersection) {}