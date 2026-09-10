#include "StatsWindow.h"

#include <SFML/Graphics.hpp>
#include <iomanip>
#include <sstream>
#include <string>

#include "Game.h"

StatsWindow::StatsWindow(Game* game, OverallStats stats,
                         sf::Vector2u windowSize)
    : _statsText(utility::Constants::defaultFont),
      _continue(game, {0.f, 0.f}, {100.f, 100.f},
                utility::Constants::defaultFont, "Continue", sf::Color::Green,
                sf::Color::Blue, sf::Color::Red),
      _stats(stats) {
  sf::Vector2f center{windowSize.x / 2.f, windowSize.y / 2.f};
  sf::Vector2f d{windowSize.x / 4.f, windowSize.y / 3.f};
  _container.setPosition(center - d);
  _container.setSize(d * 2.f);
  _statsText.setString(getStatsString());
  _statsText.setFillColor(sf::Color::Black);
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
  res << "Stats:\nCars arrived: " << std::fixed << std::setprecision(2)
      << (_stats.numberOfCars == 0 ? 1 : ((double)_stats.numberArrived) / _stats.numberOfCars) * 100 << "%";
  return res.str();
}

void StatsWindow::updateWindowSize(sf::Vector2f newSize) {
  sf::Vector2f center{newSize.x / 2.f, newSize.y / 2.f};
  sf::Vector2f d{newSize.x / 12.f, newSize.y / 4.f};
  _container.setPosition(center - d);
  _container.setSize(d * 2.f);
  sf::FloatRect textRect = _statsText.getLocalBounds();
  _statsText.setPosition(center - d * 0.9f);
  _continue.setStart({center.x - newSize.x / 16.f, center.y + d.y - 25.f});
  _continue.setSize({newSize.x / 8.f, 20});
}

void StatsWindow::update(sf::RenderWindow& rw) { _continue.update(rw); }

void StatsWindow::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_container);
  target.draw(_statsText);
  target.draw(_continue);
}
