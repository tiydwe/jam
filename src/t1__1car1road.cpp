#include "Simulation.h"

#include "utility.h"

void test1(){
  Simulation s;
  auto i = Intersection(s);
  s.addIntersection(i);
  auto r = Road(s, 5, i.getID());
  auto l = Lane(s, 100);
  r.addLane(l.getID());
  s.addRoad(r);
  auto c = Car(s, r.getID(), 0, 12, 1);
  c.setDestination(std::make_pair<size_t, size_t>(r.getID(), 95));
  s.addCar(c);
  for(int i = 0; i < 100; ++i){
    s.step(0.5);
  }
}