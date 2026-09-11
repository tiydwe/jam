#include "Game.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Layout.h"
#include "utility.h"

Game::Game(std::string filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    utility::logErr("Error opening game file! " + filepath);
    return;
  }
  std::string tmp;
  std::getline(file, tmp);
  std::unique_ptr<Layout> layout = std::make_unique<Layout>(tmp);
  std::getline(file, carSetupFilepath);
  MainWindow = sf::RenderWindow(sf::VideoMode({800, 600}), "JAM");
  _editor = std::make_unique<EditorWindow>(std::move(layout), this,
                                           MainWindow.getSize());
}

void Game::beginSimulation() {
  if (currentMode == GameScreenMode::EDIT) {
    _simulation = std::make_unique<SimulateWindow>(
        _editor->getLayout(), this, carSetupFilepath, MainWindow.getSize());
    currentMode = GameScreenMode::SIMULATE;
  }
}

void Game::endSimulation() {
  if (currentMode == GameScreenMode::SIMULATE) {
    _statsWindow = std::make_unique<StatsWindow>(this, _simulation->getResults(), MainWindow.getSize());
    currentMode = GameScreenMode::STATS;
  }
}

void Game::endStatsScreen() {
  if (currentMode == GameScreenMode::STATS){
    currentMode = GameScreenMode::EDIT;
  }
}

void Game::run() {
  lastTickTime = clk.getElapsedTime();
  while (MainWindow.isOpen()) {
    while (const std::optional event = MainWindow.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        MainWindow.close();
        return;
      }
      if (const auto* resizeEvent = event->getIf<sf::Event::Resized>()) {
        auto view = MainWindow.getView();
        sf::Vector2f newSize = {static_cast<float>(resizeEvent->size.x),
                                static_cast<float>(resizeEvent->size.y)};
        view.setSize(newSize);
        view.setCenter(newSize / 2.f);
        MainWindow.setView(view);
        if (currentMode == GameScreenMode::SIMULATE) {
          _simulation->updateWindowSize(newSize);
        } else if (currentMode == GameScreenMode::EDIT) {
          _editor->updateWindowSize(newSize);
        }
        else if(currentMode == GameScreenMode::STATS){
          _statsWindow->updateWindowSize({MainWindow.getSize().x, MainWindow.getSize().y});
        }
      }
      if (currentMode == GameScreenMode::SIMULATE) {
        _simulation->handleEvent(*event, MainWindow);
      } else if (currentMode == GameScreenMode::EDIT) {
        _editor->handleEvent(*event, MainWindow);
      } else if (currentMode == GameScreenMode::STATS) {
        // intentionally blank
      }
    }
    sf::Time nex = clk.getElapsedTime();
    double dt = std::min((nex - lastTickTime).asSeconds(), clampTime);
    if (currentMode == GameScreenMode::SIMULATE) {
      _simulation->step(dt);
      _simulation->update(MainWindow);
    } else if (currentMode == GameScreenMode::EDIT) {
      _editor->update(MainWindow);
    } else if (currentMode == GameScreenMode::STATS){
      _statsWindow->update(MainWindow);
    }
    lastTickTime = nex;
    MainWindow.clear();
    if (currentMode == GameScreenMode::SIMULATE) {
      MainWindow.draw(*_simulation.get());
    } else if (currentMode == GameScreenMode::EDIT) {
      MainWindow.draw(*_editor.get());
    } else if(currentMode == GameScreenMode::STATS) {
      MainWindow.draw(*_statsWindow.get());
    }
    MainWindow.display();
  }
}

/*
void Game::run() {
  lastTickTime = clk.getElapsedTime();
  while (MainWindow.isOpen()) {
    if (simulating) {
      while (const std::optional event = MainWindow.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
          _simulation.reset();
          // SimulationRenderWindow.close();
          simulating = false;
        }
        _simulation->handleEvent(*event, MainWindow);
      }
      if (!simulating) {
        continue;
      }
      sf::Time nex = clk.getElapsedTime();
      _simulation->step(std::min((nex - lastTickTime).asSeconds(), clampTime));
      _simulation->update(MainWindow);
      lastTickTime = nex;
      MainWindow.clear();
      MainWindow.draw(*_simulation.get());
      MainWindow.display();
    } else {
      while (const std::optional event = MainWindow.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
          MainWindow.close();
          _editor.reset();
          return;
        }
        _editor->handleEvent(*event, MainWindow);
      }
      _editor->update(MainWindow);
      MainWindow.clear();
      MainWindow.draw(*_editor.get());
      MainWindow.display();
    }
  }
}
//*/