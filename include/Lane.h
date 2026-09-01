#pragma once

#include <vector>

class Simulation;

class Lane{
  public:

  Lane();
  Lane(Simulation& parent, double length, size_t roadid);

  std::pair<double, size_t> minDistance();
  // min distance after car, returns (-1, -1) if first car
  std::pair<double, size_t> minDistance(size_t carid);

  void addCar(double dist, size_t car){_cars.push_back({dist, car});};
  void removeCar(size_t car);
  void moveCar(double newDist, size_t car);
  
  size_t getID()const{return _id;}
  size_t getLength()const{return _length;}
  size_t getRoad()const{return _roadid;}

  private:

  Simulation* _parent;

  size_t _id;
  double _length;
  size_t _roadid;
  //distance, carid
  std::vector<std::pair<double, size_t>> _cars;
};