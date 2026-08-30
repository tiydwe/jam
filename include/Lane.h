#pragma once

#include <vector>

class Simulation;

class Lane{
  public:

  Lane();
  Lane(Simulation& parent, double length);

  std::pair<double, size_t> minDistance();
  // min distance after car, returns (-1, -1) if first car
  std::pair<double, size_t> minDistance(size_t carid);

  void addCar(double dist, size_t car){_cars.push_back({dist, car});};
  void removeCar(size_t car);
  void moveCar(double newDist, size_t car);
  
  size_t getID()const{return _id;}
  size_t getLength()const{return _length;}

  private:

  Simulation* _parent;

  int _id;
  double _length;
  //distance, carid
  std::vector<std::pair<double, size_t>> _cars;
};