#pragma once

#include <vector>

class Simulation;

enum class Lights{
  RED, YELLOW, GREEN
};

class Intersection{
  public:

  Intersection();
  Intersection(Simulation& parent);

  size_t getID()const{return _id;}

  Lights getLightByLaneID(size_t laneid) const;

  const std::vector<size_t>& getOutgoings()const{return outgoings;}
  void addOutgoing(size_t road_id){outgoings.push_back(road_id);}


  private:
  Simulation* _parent;

  size_t _id;

  std::vector<size_t> outgoings;
  
  // TODO: schedule
  // for now, always green
};