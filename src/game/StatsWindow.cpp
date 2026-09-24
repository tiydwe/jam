#include "StatsWindow.h"

#include <SFML/Graphics.hpp>
#include <format>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "Game.h"

Level::Level(std::filesystem::path levelConfigPath) {
  std::ifstream file(levelConfigPath);
  if (!file.is_open()) {
    utility::logErr("Level::Level - could not open file " +
                    levelConfigPath.string());
  } else {
    std::string tmp;
    std::getline(file, tmp);
    {
      std::stringstream ss(tmp);
      ss >> _percentArrived >> _avgSpeed >> _timeWasted;
    }
  }
}

bool Level::passes(OverallStats stats) {
  if (stats.numberOfCars == 0 || stats.totalTimeTraveled == 0) {
    return false;
  }
  return stats.numberArrived / stats.numberOfCars >= _percentArrived &&
         stats.totalDistFromSourceToDest / stats.totalTimeTraveled >= _avgSpeed;
}

bool Level::pass1(OverallStats stats) const {
  if (stats.numberOfCars == 0) {
    return false;
  }
  return stats.numberArrived / stats.numberOfCars >= _percentArrived;
}

bool Level::pass2(OverallStats stats) const {
  if (stats.totalTimeTraveled == 0) {
    return false;
  }
  return stats.totalDistFromSourceToDest / stats.totalTimeTraveled >= _avgSpeed;
}

StatsWindow::StatsWindow(Game* game, OverallStats stats, Level& level,
                         sf::Vector2u windowSize)
    : _statsText(utility::Constants::defaultFont),
      _continue(game, {0.f, 0.f}, {100.f, 100.f},
                utility::Constants::defaultFont, "Continue",
                utility::ColorPalette::functionalBtn,
                utility::ColorPalette::functionalBtnHover, sf::Color::Red),
      _stats(stats),
      _level(level) {
  sf::Vector2f center{windowSize.x / 2.f, windowSize.y / 2.f};
  sf::Vector2f d{250, 300};
  _container.setPosition(center - d);
  _container.setSize(d * 2.f);
  _statsText.setString(getStatsString());
  _statsText.setFillColor(sf::Color::Black);
  _statsText.setCharacterSize(25);
  sf::FloatRect textRect = _statsText.getLocalBounds();
  _statsText.setOrigin({textRect.position.x + textRect.size.x / 2,
                        textRect.position.y + textRect.size.y / 2});
  _statsText.setPosition(center);

  _continue.setStart({center.x - windowSize.x / 6.f, center.y + d.y - 50.f});
  //_continue.setStart(center);
  _continue.setSize({windowSize.x / 3.f, 40});
  _continue.setOnclick([&](Game* game) { game->endStatsScreen(); });

  _carsArrived.setSize({400, 60});
  _carsArrived.setOrigin(_carsArrived.getSize() / 2.f);
  _carsArrived.setPosition({windowSize.x / 2.f, windowSize.y / 2.f - 85});
  _carsArrived.setFillColor(sf::Color::Transparent);
  _carsArrived.setOutlineColor(_level.pass1(stats) ? sf::Color::Green : sf::Color::Red);
  _carsArrived.setOutlineThickness(8.f);

  _averageSpeed.setSize({400, 60});
  _averageSpeed.setOrigin(_averageSpeed.getSize() / 2.f);
  _averageSpeed.setPosition({windowSize.x / 2.f, windowSize.y / 2.f});
  _averageSpeed.setFillColor(sf::Color::Transparent);
  _averageSpeed.setOutlineColor(_level.pass2(stats) ? sf::Color::Green : sf::Color::Red);
  _averageSpeed.setOutlineThickness(8.f);

  _background.setSize({(float)windowSize.x, (float)windowSize.y});
  _background.setFillColor(utility::Constants::BACKGROUND_COLOR);
}

std::string StatsWindow::getStatsString() {
  std::ostringstream res;
  res << "Percent arrived:\n"
      << std::fixed << std::setprecision(2)
      << (_stats.numberOfCars == 0
              ? 0
              : ((double)_stats.numberArrived) / _stats.numberOfCars * 100)
      << "%, required min " << _level.getPercentArrived() * 100 << "%\n\n"
      << "Average speed:\n"
      << (utility::isclose(_stats.totalTimeTraveled, 0.0)
              ? 0
              : _stats.totalDistFromSourceToDest / _stats.totalTimeTraveled)
      << ", required min " << _level.getAvgSpeed() << "\n\n\n"
      << (_level.passes(_stats)
              ? "PASSED!"
              : "You didn't meet all the requirments!");
  return res.str();
}

void StatsWindow::updateWindowSize(sf::Vector2f newSize) {
  sf::Vector2f center{newSize.x / 2.f, newSize.y / 2.f};
  sf::Vector2f d{250, 300};
  _container.setPosition(center - d);
  _container.setSize(d * 2.f);
  sf::FloatRect textRect = _statsText.getLocalBounds();
  _statsText.setOrigin({textRect.position.x + textRect.size.x / 2,
                        textRect.position.y + textRect.size.y / 2});
  _statsText.setPosition(center);
  _continue.setStart({center.x - 220, center.y + d.y - 50.f});
  _continue.setSize({440.f, 40.f});
  _background.setSize(newSize);
}

void StatsWindow::update(sf::Vector2f mousePosition, bool enable) {
  _continue.update(mousePosition, enable);
}

void StatsWindow::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_background);
  target.draw(_container);
  target.draw(_carsArrived);
  target.draw(_averageSpeed);
  target.draw(_statsText);
  target.draw(_continue);
}
