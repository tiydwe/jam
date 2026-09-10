#pragma once

#include <memory>
#include <utility>

#include "Button.h"
#include "CarPhysical.h"
#include "Layout.h"
#include "SFML/Graphics.hpp"
#include "Simulation.h"

class Game;

class SimulateWindow : public sf::Drawable, sf::Transformable {
 public:
  SimulateWindow(Layout* l, Game* g, std::string filename,
                 sf::Vector2u windowSize);

  void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

  void updateWindowSize(sf::Vector2f newSize);

  Simulation* getSimulation() const { return _s.get(); }
  OverallStats getResults() const { return _s->getStats(); }
  bool isDone() const {return _s->isDone();}

  void step(double trueDt);
  void update(sf::RenderWindow& rw);
  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;

 private:
  std::unique_ptr<Simulation> _s;
  Layout* _l;

  Game* _g;

  sf::View _uiview;
  sf::View _worldview;

  bool _isDragging = false;
  sf::Vector2i _oldMousePos;
  double _zoomLevel = 1.0;

  sf::RectangleShape _topbar;

  Button _slow;
  Button _mid;
  Button _fast;
  Button _exitSim;

  double _timeMultiplier = 5.0;
};