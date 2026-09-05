#include "Button.h"

#include "Game.h"
#include "utility.h"

Button::Button(Game* game, sf::Vector2f start, sf::Vector2f size, sf::Font font,
               std::string text, sf::Color normal, sf::Color hover,
               sf::Color click, int zVal)
    : _game(game),
      _normal(normal),
      _hover(hover),
      _click(hover),
      _start(start),
      _size(size),
      _font(font),
      _zVal(zVal),
      _isPressed(false),
      _text(utility::Constants::defaultFont, "[empty]") {
  _shape.setPosition(start);
  _shape.setSize(size);
  _shape.setFillColor(_normal);
  _text.setFont(_font);
  _text.setString(text);
  _text.setCharacterSize(18);
  _text.setFillColor(sf::Color::Black);

  sf::FloatRect textRect = _text.getLocalBounds();
  _text.setOrigin({textRect.position.x + textRect.size.x/2, textRect.position.y + textRect.size.y/2});
  _text.setPosition({start.x + size.x/2, start.y + start.y/2});

}

void Button::setOnclick(const std::function<void(Game*)>& onclick) {
  _onclick = onclick;
}

bool Button::update(const sf::RenderWindow& window) {
  auto posI = sf::Mouse::getPosition(window);
  sf::Vector2f position{(float)posI.x, (float)posI.y};
  bool mouseover = sf::FloatRect(_start, _size).contains(position);
  if (!mouseover) {
    _isPressed = false;
  } else {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
      if (!_isPressed) {
        _isPressed = true;
        _onclick(_game);
        return true;
      }
    } else {
      _isPressed = false;
    }
  }
  return false;
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_shape);
  target.draw(_text);
}
