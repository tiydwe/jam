#pragma once

#include <vector>

class Simulation;

class Intersection{
  public:

  Intersection();
  Intersection(Simulation& parent);

  size_t getID()const{return _id;}


  private:
  Simulation* _parent;

  size_t _id;

  std::vector<size_t> outgoings;
  
  // TODO: schedule
  // for now, always green
};