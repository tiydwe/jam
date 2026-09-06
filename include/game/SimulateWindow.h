#pragma once

#include "SFML/Graphics.hpp"
#include "Layout.h"
#include "Button.h"
#include "Simulation.h"

#include "CarPhysical.h"

#include <memory>
#include <utility>

class Game;

class SimulateWindow : public sf::Drawable, sf::Transformable{
  public:
  SimulateWindow(Layout* l, Game* g, std::string filename, sf::Vector2u windowSize);
  
  void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

  void updateWindowSize(sf::Vector2f newSize);

  Simulation* getSimulation() const {return _s.get();}
  
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

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
};