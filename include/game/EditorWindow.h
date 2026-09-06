#pragma once

#include <SFML/Graphics.hpp>
#include "Layout.h"
#include "Button.h"

#include <memory>
#include <utility>

class Game;

class EditorWindow : public sf::Drawable, sf::Transformable{
 public:
  EditorWindow(std::unique_ptr<Layout> l, Game* game, sf::Vector2u windowSize);

  void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

  void onclickSimulate(Game* game);

  void updateWindowSize(sf::Vector2f newSize);

  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;
  
  Layout* getLayout() const {return _l.get();}

 private:
  std::unique_ptr<Layout> _l;

  Game* _game;

  sf::View _uiview;
  sf::View _worldview;

  bool _isDragging = false;
  sf::Vector2i _oldMousePos;
  double _zoomLevel = 1.0;

  sf::RectangleShape _topbar;

  Button _simulateButton;
};