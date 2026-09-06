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
  _editor = std::make_unique<EditorWindow>(std::move(layout), this, EditorRenderWindow.getSize());
}

void Game::beginSimulation() {
  if (simulating) {
    return;
  }
  _simulation = std::make_unique<SimulateWindow>(
      _editor->getLayout(), this, carSetupFilepath,
      SimulationRenderWindow.getSize());
  simulating = true;
}

void Game::run() {
  EditorRenderWindow = sf::RenderWindow(sf::VideoMode({600, 500}), "JAM");
  lastTickTime = clk.getElapsedTime();
  while (EditorRenderWindow.isOpen()) {
    if (simulating) {
      while (const std::optional event = SimulationRenderWindow.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
          _simulation.reset();
          simulating = false;
        }
        _simulation->handleEvent(*event, SimulationRenderWindow);
      }
      sf::Time nex = clk.getElapsedTime();
      _simulation->getSimulation()->step(
          std::min((nex - lastTickTime).asSeconds(), clampTime) *
          timeMultiplier);
      lastTickTime = nex;
      SimulationRenderWindow.clear();
      SimulationRenderWindow.draw(*_simulation.get());
      SimulationRenderWindow.display();
    } else {
      while (const std::optional event = EditorRenderWindow.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
          EditorRenderWindow.close();
          _editor.reset();
          return;
        }
        _editor->handleEvent(*event, EditorRenderWindow);
      }
      EditorRenderWindow.clear();
      EditorRenderWindow.draw(*_editor->getLayout());
      EditorRenderWindow.display();
    }
  }
}
