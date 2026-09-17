#pragma once
#include <map>
#include <memory>
#include <utility>
#include <filesystem>

#include "CarPhysical.h"
#include "EditorWindow.h"
#include "IntersectionPhysical.h"
#include "RoadPhysical.h"
#include "SFML/Graphics.hpp"
#include "SimulateWindow.h"
#include "Simulation.h"
#include "utility.h"
#include "StatsWindow.h"
#include "LevelSelectWindow.h"
#include "TitleWindow.h"
#include "SaveWindow.h"

enum class GameScreenMode{
  EDIT, SIMULATE, STATS, LEVEL_SELECT, TITLE_SCREEN, LOAD_GAME
};

class Game {
 public:
  Game(std::filesystem::path pathToLevelsFolder);

  void beginSimulation();
  void endSimulation();
  void endStatsScreen();
  void endLevelSelectChoose(std::filesystem::path pathToMainDat);
  void endLevelSelectBack();
  void beginLevelSelect();
  void beginLoadFile();
  void endLoadFileBack();
  void endLoadFileChoose(std::filesystem::path pathToMainDat);

  void run();

  const sf::RenderWindow* getMainWindow() const {return &MainWindow;}

 private:
  float clampTime = 0.33;
  double timeMultiplier = 5.0;
  GameScreenMode currentMode = GameScreenMode::TITLE_SCREEN;
  sf::Clock clk;
  sf::Time lastTickTime;
  std::string carSetupFilepath;
  sf::RenderWindow MainWindow;
  sf::RenderWindow SimulationRenderWindow;
  std::unique_ptr<EditorWindow> _editor;
  std::unique_ptr<SimulateWindow> _simulation;
  std::unique_ptr<StatsWindow> _statsWindow;
  std::unique_ptr<LevelSelectWindow> _levelSelectWindow;
  std::unique_ptr<TitleWindow> _titleWindow;
  std::unique_ptr<SaveWindow> _saveWindow;
  std::filesystem::path _carP;
  std::filesystem::path _layoutP;
  std::filesystem::path _mainP;
  std::filesystem::path _progressP;
  std::filesystem::path _victoryP;
  std::filesystem::path _pathToLevelsFolder;
};