#pragma once

#include <SFML/Graphics.hpp>

#include <filesystem>

#include "utility.h"

class Building : public sf::Drawable, sf::Transformable{
  public:
  Building(std::filesystem::path datapath, sf::Vector2f pos);
  sf::RectangleShape getHitbox() const;

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

  void setDarkened(bool darkened) {_darkened = darkened;}
  void setShow(bool show) {_show = show;}

  private:

  float _scale = 1.0;

  sf::Texture _texture;
  sf::Vector2f _pos;

  bool _darkened = false;
  bool _show = true;
};