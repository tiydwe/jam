#pragma once
#include "Simulation.h"

#include "utility.h"

void test2(){
  Simulation s;
  auto i = Intersection(s);
  auto r = Road(s, 5, i.getID());
  auto l = Lane(s, 50);
  r.addLane(l.getID());
  r.setEdgeLane(l.getID());
  auto i2 = Intersection(s);
  auto r2 = Road(s, 5, i2.getID());
  auto l2 = Lane(s, 100);
  r2.addLane(l2.getID());
  r2.setEdgeLane(l2.getID());
  i.addOutgoing(r2.getID());
  s.addIntersection(i);
  s.addRoad(r);
  s.addLane(l);
  s.addIntersection(i2);
  s.addRoad(r2);
  s.addLane(l2);
  
  auto c = Car(s, r.getID(), l.getID(), 12, 1);
  c.setDestination(std::make_pair<size_t, size_t>(r2.getID(), 95));
  c.setRoute({r2.getID()});
  s.addCar(c);
  for(int i = 0; i < 100; ++i){
    s.step(0.5);
  }
}