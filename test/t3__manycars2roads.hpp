#pragma once
#include "Simulation.h"

#include "utility.h"

void test3(){
  Simulation s;
  auto i = Intersection(s);
  auto r = Road(s, 5, i.getID());
  auto l = Lane(s, 50);
  r.addLane(l.getID());
  r.setEdgeLane(l.getID());
  auto i2 = Intersection(s);
  auto r2 = Road(s, 5, i2.getID());
  auto l2 = Lane(s, 150);
  r2.addLane(l2.getID());
  r2.setEdgeLane(l2.getID());
  i.addOutgoing(r2.getID());
  
  auto c = Car(s, r.getID(), l.getID(), 1, 1);
  c.setDestination(std::make_pair<size_t, size_t>(r2.getID(), 85));
  c.setRoute({r2.getID()});
  l.addCar(c.getDistance(), c.getID());
  s.addCar(c);
  auto c2 = Car(s, r.getID(), l.getID(), 5, 1.5);
  c2.setDestination(std::make_pair<size_t, size_t>(r2.getID(), 90));
  c2.setRoute({r2.getID()});
  l.addCar(c2.getDistance(), c2.getID());
  s.addCar(c2);
  auto c3 = Car(s, r.getID(), l.getID(), 10, 0.5);
  c3.setDestination(std::make_pair<size_t, size_t>(r2.getID(), 95));
  c3.setRoute({r2.getID()});
  l.addCar(c3.getDistance(), c3.getID());
  s.addCar(c3);
  
  s.addIntersection(i);
  s.addRoad(r);
  s.addLane(l);
  s.addIntersection(i2);
  s.addRoad(r2);
  s.addLane(l2);
  for(int i = 0; i < 50; ++i){
    s.step(0.8);
  }
}