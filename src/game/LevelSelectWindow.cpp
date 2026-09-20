#include "LevelSelectWindow.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
#include "Game.h"


LevelSelectWindow::LevelSelectWindow(std::filesystem::path levelsDir,
                                     sf::Vector2f windowSize, Game* game)
    : _game(game),
      _back(nullptr, {windowSize.x / 2.f - 40, windowSize.y / 2.f + 70},
            {80, 30}, utility::Constants::defaultFont, "BACK",
            sf::Color(120, 120, 120), sf::Color(100, 100, 100),
            sf::Color::Black),
      _title(utility::Constants::defaultFont, "Select a level:") {
  _back.setOnclick([this](Game* gm) { this->onclickBack(); });
  _title.setOrigin(_title.getLocalBounds().getCenter());
  _title.setPosition({windowSize.x / 2.f, windowSize.y / 2.f - 40});
  _title.setFillColor(sf::Color::Black);
  if (!std::filesystem::exists(levelsDir) ||
      !std::filesystem::is_directory(levelsDir)) {
    utility::logErr(
        "LevelSelectWindow::LevelSelectWindow - path is not a directory or "
        "does not exist: " +
        levelsDir.string());
    utility::exit();
  }
  for (const auto& levelMainFile :
       std::filesystem::recursive_directory_iterator(levelsDir)) {
    if (levelMainFile.is_regular_file() &&
        levelMainFile.path().filename() == "main.dat") {
      std::ifstream file(levelMainFile.path());
      std::string progressFile;
      std::getline(file, progressFile);
      std::getline(file, progressFile);
      std::getline(file, progressFile);
      std::getline(file, progressFile);
      file = std::ifstream(progressFile);
      std::string tmp;
      std::getline(file, tmp);
      bool complete;
      {
        std::stringstream ss(tmp);
        char c;
        ss >> c;
        complete = c == 'Y';
      }
      _levels.push_back({levelMainFile, complete ? LevelStatus::COMPLETE : LevelStatus::INCOMPLETE});
    }
  }
  this->createButtons(windowSize);
}

void LevelSelectWindow::updateWindowSize(sf::Vector2f newSize) {
  _levelButtons.clear();
  this->createButtons(newSize);
  _back.setStart({newSize.x / 2.f - 40, newSize.y / 2.f - 100});
  _contentBox.setPosition(newSize/ 2.f);
  _title.setPosition({newSize.x / 2.f, newSize.y / 2.f - 40});
}

void LevelSelectWindow::update(sf::Vector2f mousePos) {
  for (const auto& x : _levelButtons) {
    x->update(mousePos);
  }
  _back.update(mousePos);
}

std::filesystem::path LevelSelectWindow::getChoosenLevel() const {
  return _choosenLevel;
}

void LevelSelectWindow::draw(sf::RenderTarget& target,
                             sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_contentBox,states);
  target.draw(_title, states);
  for (const auto& x : _levelButtons) {
    x->draw(target, states);
  }
  _back.draw(target, states);
}

void LevelSelectWindow::onclickLevel(std::filesystem::path levelPath) {
  _choosenLevel = levelPath;
  _game->endLevelSelectChoose(levelPath);
}

void LevelSelectWindow::onclickBack() {
  _game->endLevelSelectBack();
}

void LevelSelectWindow::createButtons(sf::Vector2f windowSize) {
  sf::Vector2f center = windowSize / 2.f;
  double height = 50.f;
  double widthPerButton = 70.f;
  double spacing = 15.f;
  double netWidth =
      _levels.size() * widthPerButton + (_levels.size() - 1) * spacing;
  _contentBox.setSize({netWidth + 2 * spacing, height + 180});
  _contentBox.setOrigin(_contentBox.getSize() / 2.f);
  _contentBox.setPosition(windowSize/ 2.f);
  sf::Vector2f startPos{center.x - netWidth / 2, center.y - height / 2};
  sf::Vector2f size{height, widthPerButton};
  for (const auto& levelPath : _levels) {
    _levelButtons.push_back(std::make_unique<Button>(
        nullptr, startPos, size, utility::Constants::defaultFont,
        levelPath.first.parent_path().filename().string(), sf::Color(120, 120, 120),
        sf::Color(100, 100, 100), sf::Color::Black));
    if(levelPath.second == LevelStatus::COMPLETE){
      _levelButtons.back()->setNormal(sf::Color(120, 150, 120));
      _levelButtons.back()->setHover(sf::Color(100, 130, 100));
    }
    _levelButtons.back()->setOnclick(
        [this, levelPath](Game* gm) { this->onclickLevel(levelPath.first); });
    startPos.x += widthPerButton + spacing;
  }
}
