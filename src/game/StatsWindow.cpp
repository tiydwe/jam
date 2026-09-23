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
         stats.totalDistFromSourceToDest / stats.totalTimeTraveled >= _avgSpeed &&
         (stats.totalTimeAtIntersection + stats.totalTimeWaitingNextCar) /
                 stats.totalTimeTraveled <=
             _timeWasted;
}

StatsWindow::StatsWindow(Game* game, OverallStats stats, Level& level,
                         sf::Vector2u windowSize)
    : _statsText(utility::Constants::defaultFont),
      _continue(game, {0.f, 0.f}, {100.f, 100.f},
                utility::Constants::defaultFont, "Continue", sf::Color::Green,
                sf::Color::Blue, sf::Color::Red),
      _stats(stats),
      _level(level) {
  sf::Vector2f center{windowSize.x / 2.f, windowSize.y / 2.f};
  sf::Vector2f d{windowSize.x / 4.f, windowSize.y / 3.f};
  _container.setPosition(center - d);
  _container.setSize(d * 2.f);
  _statsText.setString(getStatsString());
  _statsText.setFillColor(sf::Color::Black);
  _statsText.setCharacterSize(20);
  sf::FloatRect textRect = _statsText.getLocalBounds();
  _statsText.setOrigin({textRect.position.x + textRect.size.x / 2,
                        textRect.position.y + textRect.size.y / 2});
  _statsText.setPosition(center);

  _continue.setStart({center.x - windowSize.x / 6.f, center.y + d.y - 50.f});
  //_continue.setStart(center);
  _continue.setSize({windowSize.x / 3.f, 40});
  _continue.setOnclick([&](Game* game) { game->endStatsScreen(); });
}

std::string StatsWindow::getStatsString() {
  std::ostringstream res;
  res << "Stats:\nPercent arrived: " << std::fixed << std::setprecision(2)
      << (_stats.numberOfCars == 0
              ? 0
              : ((double)_stats.numberArrived) / _stats.numberOfCars * 100)
      << "%, required min " << _level.getPercentArrived() * 100 << "%\n"
      << "Average speed: "
      << (utility::isclose(_stats.totalTimeTraveled, 0.0)
              ? 0
              : _stats.totalDistFromSourceToDest / _stats.totalTimeTraveled)
      << ", required min " << _level.getAvgSpeed() << "\n"
      << "Time wasted: "
      << (utility::isclose(_stats.totalTimeTraveled, 0.0)
              ? 0
              : (_stats.totalTimeAtIntersection +
                 _stats.totalTimeWaitingNextCar) /
                    _stats.totalTimeTraveled) *
             100
      << "%, required max " << _level.getTimeWasted()*100 << "%\n\n"
      << (_level.passes(_stats)
              ? "PASSED!"
              : "You didn't meet all the requirments! Try again.");
  return res.str();
}

void StatsWindow::updateWindowSize(sf::Vector2f newSize) {
  sf::Vector2f center{newSize.x / 2.f, newSize.y / 2.f};
  sf::Vector2f d{newSize.x / 4.f, newSize.y / 3.f};
  _container.setPosition(center - d);
  _container.setSize(d * 2.f);
  sf::FloatRect textRect = _statsText.getLocalBounds();
  _statsText.setOrigin({textRect.position.x + textRect.size.x / 2,
                        textRect.position.y + textRect.size.y / 2});
  _statsText.setPosition(center);
  _continue.setStart({center.x - newSize.x / 6.f, center.y + d.y - 50.f});
  _continue.setSize({newSize.x / 3.f, 40.f});
}

void StatsWindow::update(sf::Vector2f mousePosition, bool enable) {
  _continue.update(mousePosition, enable);
}

void StatsWindow::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_container);
  target.draw(_statsText);
  target.draw(_continue);
}
