#pragma once
#include <map>
#include <memory>
#include <utility>

#include "CarPhysical.h"
#include "EditorWindow.h"
#include "IntersectionPhysical.h"
#include "RoadPhysical.h"
#include "SFML/Graphics.hpp"
#include "SimulateWindow.h"
#include "Simulation.h"
#include "utility.h"
#include "StatsWindow.h"

enum class GameScreenMode{
  EDIT, SIMULATE, STATS
};

class Game {
 public:
  Game(std::string filepath);

  void beginSimulation();
  void endSimulation();
  void endStatsScreen();

  void run();

 private:
  float clampTime = 0.33;
  double timeMultiplier = 5.0;
  GameScreenMode currentMode = GameScreenMode::EDIT;
  sf::Clock clk;
  sf::Time lastTickTime;
  std::string carSetupFilepath;
  sf::RenderWindow MainWindow;
  sf::RenderWindow SimulationRenderWindow;
  std::unique_ptr<EditorWindow> _editor;
  std::unique_ptr<SimulateWindow> _simulation;
  std::unique_ptr<StatsWindow> _statsWindow;
};