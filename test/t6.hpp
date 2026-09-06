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
  g.run();

}