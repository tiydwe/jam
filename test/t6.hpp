#include "Game.h"

#pragma once
#include <SFML/Graphics.hpp>

#include "Car.h"
#include "RoadPhysical.h"
#include "CarPhysical.h"
#include "Simulation.h"
#include "utility.h"

void test6() {
  
  Game g("assets/presets/games/t6.dat");

  double timeMultiplier = 10.0;
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
    g.step(std::min((nex - last).asSeconds(), clampTime) * timeMultiplier);
    last = nex;
    window.clear();
    window.draw(g);
    window.display();
  }
}