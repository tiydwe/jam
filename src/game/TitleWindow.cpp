#include "TitleWindow.h"

#include "Game.h"

TitleWindow::TitleWindow(Game* game, sf::Vector2f size)
    : _game(game),
      _title(utility::Constants::defaultFont, "JAM"),
      _backgroundsprite(utility::Constants::titleScreenBackground),
      _load(game, {0.f, 0.f}, {200.f, 50.f}, utility::Constants::defaultFont,
            "Load Saved Game", utility::ColorPalette::functionalBtn,
            utility::ColorPalette::functionalBtnHover, sf::Color::Red),
      _newLevel(game, {0.f, 0.f}, {200.f, 50.f},
                utility::Constants::defaultFont, "New Game",
                utility::ColorPalette::functionalBtn,
                utility::ColorPalette::functionalBtnHover, sf::Color::Red) {
  sf::Vector2f texturesize{
      static_cast<float>(utility::Constants::titleScreenBackground.getSize().x),
      static_cast<float>(
          utility::Constants::titleScreenBackground.getSize().y)};
  float scale = std::max(size.x / texturesize.x, size.y / texturesize.y);
  _backgroundsprite.setScale({scale, scale});
  sf::Vector2f corrsize{size.x / scale, size.y / scale};
  sf::Vector2f startpos = ((texturesize - corrsize) / 2.f);
  _backgroundsprite.setTextureRect(sf::IntRect{
      sf::Vector2i{static_cast<int>(startpos.x), static_cast<int>(startpos.y)},
      sf::Vector2i{static_cast<int>(corrsize.x), static_cast<int>(corrsize.y)}});
  _contentBox.setSize({250.f, 300.f});
  _contentBox.setOrigin(_contentBox.getSize() / 2.f);
  _contentBox.setPosition(size / 2.f);
  _title.setFillColor(sf::Color::Black);
  _title.setOrigin(_title.getGlobalBounds().getCenter());
  _title.setPosition({size.x / 2.f, size.y / 2.f - 80});
  _load.setStart({size.x / 2.f - 100, size.y / 2.f});
  _newLevel.setStart({size.x / 2.f - 100, size.y / 2.f + 60});
  _load.setOnclick([this](Game* game) { this->_game->beginLoadFile(); });
  _newLevel.setOnclick([this](Game* game) { this->_game->beginLevelSelect(); });
}

void TitleWindow::updateWindowSize(sf::Vector2f newSize) {
  sf::Vector2f texturesize{
      static_cast<float>(utility::Constants::titleScreenBackground.getSize().x),
      static_cast<float>(
          utility::Constants::titleScreenBackground.getSize().y)};
  float scale = std::max(newSize.x / texturesize.x, newSize.y / texturesize.y);
  _backgroundsprite.setScale({scale, scale});
  sf::Vector2f corrsize{newSize.x / scale, newSize.y / scale};
  sf::Vector2f startpos = ((texturesize - corrsize) / 2.f);
  _backgroundsprite.setTextureRect(sf::IntRect{
      sf::Vector2i{static_cast<int>(startpos.x), static_cast<int>(startpos.y)},
      sf::Vector2i{static_cast<int>(corrsize.x), static_cast<int>(corrsize.y)}});

  _title.setPosition({newSize.x / 2.f, newSize.y / 2.f - 80});
  _contentBox.setPosition(newSize / 2.f);
  _load.setStart({newSize.x / 2.f - 100, newSize.y / 2.f});
  _newLevel.setStart({newSize.x / 2.f - 100, newSize.y / 2.f + 60});
}

void TitleWindow::update(sf::Vector2f mousePos, bool enable) {
  _load.update(mousePos, enable);
  _newLevel.update(mousePos, enable);
}

void TitleWindow::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_backgroundsprite, states);
  target.draw(_contentBox, states);
  _load.draw(target, states);
  _newLevel.draw(target, states);
  target.draw(_title, states);
}
