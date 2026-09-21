#include "Game.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Layout.h"
#include "utility.h"

Game::Game(std::filesystem::path pathToLevelsFolder)
    : _pathToLevelsFolder(pathToLevelsFolder) {
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 8;
  MainWindow =
      sf::RenderWindow(sf::VideoMode({1200, 1000}), "JAM", sf::Style::Default,
                       sf::State::Windowed, settings);
  _titleWindow = std::make_unique<TitleWindow>(
      this, sf::Vector2f{static_cast<float>(MainWindow.getSize().x),
                         static_cast<float>(MainWindow.getSize().y)});
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
    Level level(_victoryP);
    if (level.passes(_simulation->getResults())) {
      std::ofstream file(_progressP);
      file << "Y";
    }
    _statsWindow = std::make_unique<StatsWindow>(
        this, _simulation->getResults(), level, MainWindow.getSize());
    currentMode = GameScreenMode::STATS;
  }
}

void Game::endStatsScreen() {
  if (currentMode == GameScreenMode::STATS) {
    currentMode = GameScreenMode::EDIT;
  }
}

void Game::endLevelSelectChoose(std::filesystem::path pathToMainDat) {
  if (currentMode == GameScreenMode::LEVEL_SELECT) {
    std::ifstream file(pathToMainDat);
    if (!file.is_open()) {
      utility::logErr("Error opening game file! " + pathToMainDat.string());
      return;
    }
    std::string tmp;
    std::getline(file, tmp);
    std::getline(file, carSetupFilepath);
    _layoutP = std::filesystem::path(tmp);
    _carP = std::filesystem::path(carSetupFilepath);
    std::getline(file, tmp);
    _victoryP = std::filesystem::path(tmp);
    std::getline(file, tmp);
    _progressP = std::filesystem::path(tmp);
    std::unique_ptr<Layout> layout =
        std::make_unique<Layout>(_layoutP, _carP, _victoryP, _progressP);
    _editor = std::make_unique<EditorWindow>(std::move(layout), this,
                                             MainWindow.getSize());
    _mainP = pathToMainDat;
    currentMode = GameScreenMode::EDIT;
  }
}

void Game::endLevelSelectBack() {
  if (currentMode == GameScreenMode::LEVEL_SELECT) {
    currentMode = GameScreenMode::TITLE_SCREEN;
  }
}

void Game::beginLevelSelect() {
  if (currentMode == GameScreenMode::TITLE_SCREEN) {
    //utility::log("click");
    currentMode = GameScreenMode::LEVEL_SELECT;
    _levelSelectWindow = std::make_unique<LevelSelectWindow>(
        _pathToLevelsFolder,
        sf::Vector2f{static_cast<float>(MainWindow.getSize().x),
                     static_cast<float>(MainWindow.getSize().y)},
        this);
    
  }
}

void Game::beginLoadFile() {
  if (currentMode == GameScreenMode::TITLE_SCREEN) {
    _saveWindow = std::make_unique<SaveWindow>(nullptr);
    _saveWindow->setSize({MainWindow.getSize().x, MainWindow.getSize().y});
    currentMode = GameScreenMode::LOAD_GAME;
  }
}

void Game::endLoadFileBack() {
  if (currentMode == GameScreenMode::LOAD_GAME) {
    currentMode = GameScreenMode::TITLE_SCREEN;
  }
}

void Game::endLoadFileChoose(std::filesystem::path pathToMainDat) {
  if (currentMode == GameScreenMode::LOAD_GAME) {
    pathToMainDat = pathToMainDat / "main.dat";
    std::ifstream file(pathToMainDat);
    if (!file.is_open()) {
      utility::logErr("Error opening game file! " + pathToMainDat.string());
      pfd::message("JAM", "Error opening file!", pfd::choice::ok,
                   pfd::icon::error);
      currentMode = GameScreenMode::TITLE_SCREEN;
      return;
    }
    std::string tmp;
    std::getline(file, tmp);
    std::getline(file, carSetupFilepath);
    _layoutP = std::filesystem::path(tmp);
    _carP = std::filesystem::path(carSetupFilepath);
    std::getline(file, tmp);
    _victoryP = std::filesystem::path(tmp);
    std::getline(file, tmp);
    _progressP = std::filesystem::path(tmp);
    std::unique_ptr<Layout> layout =
        std::make_unique<Layout>(_layoutP, _carP, _victoryP, _progressP);
    _editor = std::make_unique<EditorWindow>(std::move(layout), this,
                                             MainWindow.getSize());
    _mainP = pathToMainDat;
    currentMode = GameScreenMode::EDIT;
  }
}

void Game::endEditorWindow() {
  if (currentMode == GameScreenMode::EDIT) {
    if (pfd::message("JAM", "Exiting will lose all unsaved changes! Exit?",
                     pfd::choice::yes_no, pfd::icon::question)
            .result() == pfd::button::yes) {
      currentMode = GameScreenMode::TITLE_SCREEN;
    }
  }
}

void Game::backSimulation()
{
  if(currentMode == GameScreenMode::SIMULATE){
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
        } else if (currentMode == GameScreenMode::STATS) {
          _statsWindow->updateWindowSize(
              {MainWindow.getSize().x, MainWindow.getSize().y});
        } else if (currentMode == GameScreenMode::LEVEL_SELECT) {
          _levelSelectWindow->updateWindowSize(newSize);
        } else if (currentMode == GameScreenMode::TITLE_SCREEN) {
          _titleWindow->updateWindowSize(newSize);
        } else if (currentMode == GameScreenMode::LOAD_GAME) {
          _saveWindow->setSize(
              {MainWindow.getSize().x, MainWindow.getSize().y});
        }
      }
      if (currentMode == GameScreenMode::SIMULATE) {
        _simulation->handleEvent(*event, MainWindow);
      } else if (currentMode == GameScreenMode::EDIT) {
        _editor->handleEvent(*event, MainWindow);
      } else if (currentMode == GameScreenMode::STATS) {
        // intentionally blank
      } else if (currentMode == GameScreenMode::LEVEL_SELECT) {
        // also blank
      } else if (currentMode == GameScreenMode::TITLE_SCREEN) {
        // also blank
      } else if (currentMode == GameScreenMode::LOAD_GAME) {
        // also blank
      }
    }
    sf::Time nex = clk.getElapsedTime();
    double dt = std::min((nex - lastTickTime).asSeconds(), clampTime);
    if (currentMode == GameScreenMode::SIMULATE) {
      _simulation->step(dt);
      _simulation->update(
          MainWindow.mapPixelToCoords(sf::Mouse::getPosition(MainWindow)));
    } else if (currentMode == GameScreenMode::EDIT) {
      _editor->update(
          MainWindow.mapPixelToCoords(sf::Mouse::getPosition(MainWindow)));
    } else if (currentMode == GameScreenMode::STATS) {
      _statsWindow->update(
          MainWindow.mapPixelToCoords(sf::Mouse::getPosition(MainWindow)));
    } else if (currentMode == GameScreenMode::LEVEL_SELECT) {
      _levelSelectWindow->update(
          MainWindow.mapPixelToCoords(sf::Mouse::getPosition(MainWindow)));
    } else if (currentMode == GameScreenMode::TITLE_SCREEN) {
      _titleWindow->update(
          MainWindow.mapPixelToCoords(sf::Mouse::getPosition(MainWindow)));
    } else if (currentMode == GameScreenMode::LOAD_GAME) {
      _saveWindow->update(
          MainWindow.mapPixelToCoords(sf::Mouse::getPosition(MainWindow)));
      if (_saveWindow->getStatus() == SaveWindowStatus::EXIT_CANCEL) {
        this->endLoadFileBack();
      } else if (_saveWindow->getStatus() == SaveWindowStatus::EXIT_DONE) {
        this->endLoadFileChoose(_saveWindow->getResult());
      }
    }
    lastTickTime = nex;
    MainWindow.clear();
    if (currentMode == GameScreenMode::SIMULATE) {
      MainWindow.draw(*_simulation.get());
    } else if (currentMode == GameScreenMode::EDIT) {
      MainWindow.draw(*_editor.get());
    } else if (currentMode == GameScreenMode::STATS) {
      MainWindow.draw(*_statsWindow.get());
    } else if (currentMode == GameScreenMode::LEVEL_SELECT) {
      MainWindow.draw(*_levelSelectWindow.get());
    } else if (currentMode == GameScreenMode::TITLE_SCREEN) {
      MainWindow.draw(*_titleWindow.get());
    } else if (currentMode == GameScreenMode::LOAD_GAME) {
      MainWindow.draw(*_saveWindow.get());
    }
    MainWindow.display();
  }
}
