#pragma once

#include "Button.h"
#include "Layout.h"
#include "SFML/Graphics.hpp"
#include "Simulation.h"

class Game;

class EditorWindow : public sf::Drawable, sf::Transformable {
 public:
  EditorWindow(Layout* l, Game* g, sf::Vector2u windowSize);

  void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

  void onclickSimulate(Game* game);

  void updateWindowSize(sf::Vector2f newSize);

  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;

 private:
  Layout* _l;

  Game* _g;

  sf::View _uiview;
  sf::View _worldview;

  bool _isDragging = false;
  sf::Vector2i _oldMousePos;
  double _zoomLevel = 1.0;

  sf::RectangleShape _topbar;

  Button _simulateButton;
};