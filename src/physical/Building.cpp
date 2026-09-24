#include "Building.h"

#include <fstream>
#include <istream>
#include <sstream>
#include <string>

Building::Building(std::filesystem::path datapath, sf::Vector2f pos)
    : _pos(pos) {
  std::ifstream file(datapath);
  std::string pathtexture;
  std::getline(file, pathtexture);
  _texture.loadFromFile(pathtexture);
  std::string tmp;
  std::getline(file, tmp);
  {
    std::stringstream ss(tmp);
    ss >> _scale;
  }
}

sf::RectangleShape Building::getHitbox() const {
  sf::RectangleShape rect;

  rect.setSize({static_cast<float>(_texture.getSize().x),
                static_cast<float>(_texture.getSize().y)});
  rect.setOrigin(rect.getSize() / 2.f);
  rect.setPosition(_pos);
  rect.setScale({_scale, _scale});

  return rect;
}

void Building::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  if (_show) {
    sf::Sprite sprite(_texture);
    sprite.setPosition(_pos);
    sprite.setScale({_scale, _scale});
    sprite.setColor(_darkened ? sf::Color(200, 200, 200) : sf::Color::White);
    target.draw(sprite, states);
  }
}
