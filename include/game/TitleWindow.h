#pragma once

#include <SFML/Graphics.hpp>

#include "Button.h"
#include "utility.h"

class Game;

class TitleWindow : public sf::Drawable, sf::Transformable {
 public:
  TitleWindow(Game* game, sf::Vector2f size);
  void updateWindowSize(sf::Vector2f newSize);
  void update(sf::Vector2f mousePos, bool enable);
  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;

 private:
  Game* _game;
  sf::RectangleShape _contentBox;
  sf::Text _title;
  Button _load;
  Button _newLevel;
};