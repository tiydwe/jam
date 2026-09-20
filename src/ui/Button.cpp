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
      _isPressed(true),
      _text(utility::Constants::defaultFont, "[empty]") {
  _shape.setPosition(start);
  _shape.setSize(size);
  _shape.setFillColor(_normal);
  _text.setFont(_font);
  _text.setString(text);
  _text.setCharacterSize(18);
  _text.setFillColor(sf::Color::Black);

  sf::FloatRect textRect = _text.getLocalBounds();
  _text.setOrigin({textRect.position.x + textRect.size.x / 2,
                   textRect.position.y + textRect.size.y / 2});
  _text.setPosition({start.x + size.x / 2, start.y + size.y / 2});
}

void Button::setOnclick(const std::function<void(Game*)>& onclick) {
  _onclick = onclick;
}

void Button::setStart(sf::Vector2f newStart) {
  _start = newStart;
  sf::FloatRect textRect = _text.getLocalBounds();
  _shape.setPosition(newStart);
  _text.setOrigin({textRect.position.x + textRect.size.x / 2,
                   textRect.position.y + textRect.size.y / 2});
  _text.setPosition({newStart.x + _size.x / 2, newStart.y + _size.y / 2});
}

void Button::setSize(sf::Vector2f newsize) {
  _size = newsize;
  sf::FloatRect textRect = _text.getLocalBounds();
  _shape.setSize(newsize);
  _text.setOrigin({textRect.position.x + textRect.size.x / 2,
                   textRect.position.y + textRect.size.y / 2});
  _text.setPosition({_start.x + newsize.x / 2, _start.y + newsize.y / 2});
}

void Button::setNormal(sf::Color color) { _normal = color; }

void Button::setHover(sf::Color color) { _hover = color; }

bool Button::update(sf::Vector2f mousePosition) {
  bool mouseover = sf::FloatRect(_start, _size).contains(mousePosition);
  if (!mouseover) {
    _shape.setFillColor(_normal);
    _isPressed = false;
  } else {
    _shape.setFillColor(_hover);
    //utility::log(std::string(_isPressed ? "yes" : "no"));
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
      //utility::log(std::string("clicked2: ") + std::string(_isPressed ? "yes" : "no"));
      _shape.setFillColor(_click);
      if (!_isPressed) {
        //utility::log("clicked3");
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
  target.draw(_shape, states);
  target.draw(_text, states);
}