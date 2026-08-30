#pragma once

#include <vector>

class Simulation;

class Lane{
  public:

  Lane();
  Lane(Simulation& parent, double length);
  
  size_t getID()const{return _id;}
  size_t getLength()const{return _length;}


  private:

  Simulation* _parent;

  int _id;
  double _length;
  std::vector<std::pair<size_t, size_t>> _cars;
};