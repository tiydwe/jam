#include "Intersection.h"

#include "utility.h"
#include "Simulation.h"

Intersection::Intersection() {
  utility::exit("This should never be called (probally something messed up in map!)");
}

Intersection::Intersection(Simulation& parent)
  : _parent(&parent),
    _id(utility::uid()) {}