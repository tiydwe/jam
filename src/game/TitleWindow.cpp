#include "TitleWindow.h"

#include "Game.h"

TitleWindow::TitleWindow(Game* game, sf::Vector2f size)
    : _game(game),
      _title(utility::Constants::defaultFont, "JAM"),
      _load(game, {0.f, 0.f}, {200.f, 50.f}, utility::Constants::defaultFont,
            "Load Saved Game", utility::ColorPalette::functionalBtn,
            utility::ColorPalette::functionalBtnHover, sf::Color::Red),
      _newLevel(game, {0.f, 0.f}, {200.f, 50.f},
                utility::Constants::defaultFont, "New Game",
                utility::ColorPalette::functionalBtn,
                utility::ColorPalette::functionalBtnHover, sf::Color::Red) {
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
  target.draw(_contentBox, states);
  _load.draw(target, states);
  _newLevel.draw(target, states);
  target.draw(_title, states);
}
