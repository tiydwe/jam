#pragma once

#include <functional>
#include <string>

#include "SFML/Graphics.hpp"

class Game;

class Button : public sf::Drawable, sf::Transformable {
 public:
  // size vector is vector from start to end corner
  Button(Game* game, sf::Vector2f start, sf::Vector2f size, sf::Font textFont,
    std::string text, sf::Color normal, sf::Color hover, sf::Color click,
    int zVal = 0);

  void setOnclick(const std::function<void(Game*)>& onclick);
  void setStart(sf::Vector2f newStart);
  void setSize(sf::Vector2f newsize);
  void setNormal(sf::Color color);

  // runs _onclick if clicked, returns true
  bool update(const sf::RenderWindow& window);
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

 private:

  Game* _game;
  std::function<void(Game*)> _onclick;

  sf::Color _normal;
  sf::Color _hover;
  sf::Color _click;

  sf::Vector2f _start;
  sf::Vector2f _size;

  sf::Font _font;

  sf::Text _text;
  sf::RectangleShape _shape;

  int _zVal;

  bool _isPressed;
};