#include "Lane.h"

#include "utility.h"
#include "Simulation.h"

Lane::Lane() {
  utility::exit("This should never be called (probally something messed up in map!)");
}

Lane::Lane(Simulation& parent, double length)
  : _parent(&parent),
    _id(utility::uid()),
    _length(length) {}