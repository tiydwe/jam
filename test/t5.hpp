#pragma once
#include <SFML/Graphics.hpp>

#include "Car.h"
#include "RoadPhysical.h"
#include "CarPhysical.h"
#include "Simulation.h"
#include "utility.h"

void test5() {
  Simulation s(42);
  auto i = Intersection(s);
  auto ib = Intersection(s);
  auto r = Road(s, 10, i.getID(), 1.5);
  auto l = Lane(s, 100, r.getID());
  auto l2 = Lane(s, 100, r.getID());
  auto rb = Road(s, 10, ib.getID(), 1.501);
  auto lb = Lane(s, 100, rb.getID());
  auto l2b = Lane(s, 100, rb.getID());
  utility::log("Road " + std::to_string(r.getID()));
  rb.addLane(lb.getID());
  rb.addLane(l2b.getID());
  s.addRoad(rb);
  r.addLane(l.getID());
  r.addLane(l2.getID());
  s.addRoad(r);
  auto i2 = Intersection(s);
  auto r2 = Road(s, 5, i2.getID(), 0.5);
  auto l3 = Lane(s, 50, r2.getID());
  auto l3b = Lane(s, 50, r2.getID());
  utility::log("Road " + std::to_string(r2.getID()));
  r2.addLane(l3.getID());
  r2.addLane(l3b.getID());
  s.addRoad(r2);
  i.addOutgoing(r2.getID());
  i.addIngoing(r.getID());
  i2.addIngoing(r2.getID());
  auto r3 = Road(s, 5, i2.getID(), 2.5);
  auto l4 = Lane(s, 50, r3.getID());
  auto l4b = Lane(s, 50, r3.getID());
  r3.addLane(l4.getID());
  r3.addLane(l4b.getID());
  s.addRoad(r3);
  utility::log("Road " + std::to_string(r3.getID()));
  i.addOutgoing(r3.getID());
  i2.addIngoing(r3.getID());
  i.getTrafficLight().reSchedule(10, 3);
  i.getTrafficLight().normalizeSchedule();
  i2.getTrafficLight().reSchedule(10, 3);
  auto c = Car(s, r.getID(), l2.getID(), (size_t)5, 1.0, 1.0);
  c.setDestination({r3.getID(), 40});
  c.setRoute({r3.getID()});
  l.addCar(c.getDistance(), c.getID());

  s.addIntersection(i);
  s.addIntersection(i2);
  s.addLane(l);
  s.addLane(l2);
  s.addLane(lb);
  s.addLane(l2b);
  s.addLane(l3);
  s.addLane(l4);
  s.addLane(l3b);
  s.addLane(l4b);
  s.addCar(c);


  RoadAsset ra("assets/data/roads/generic4lane");
  RoadPhysical rp(&s.getRoad(r.getID()), &s.getRoad(rb.getID()), ra, sf::Vector2i(250, 250),
                  sf::Vector2i(500, 150));

  CarPhysical cp(&s.getCar(c.getID()), &rp, "assets/textures/cars/car.png");

  double timeMultiplier = 5.0;
  float clampTime = 0.0333;

  sf::RenderWindow window(sf::VideoMode({500, 500}), "Test 5");
  sf::Clock ctime;
  sf::Time last = ctime.getElapsedTime();
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto* newSize = event->getIf<sf::Event::Resized>()) {
        sf::FloatRect visibleArea({0.f, 0.f}, {newSize->size.x,
                                  newSize->size.y});
        window.setView(sf::View(visibleArea));
      }
    }
    sf::Time nex = ctime.getElapsedTime();
    s.step(std::min((nex - last).asSeconds(), clampTime) * timeMultiplier);
    cp.update();
    last = nex;
    window.clear();
    window.draw(rp);
    window.draw(cp);
    window.display();
  }
}