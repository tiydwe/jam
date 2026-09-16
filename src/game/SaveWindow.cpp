#include "SaveWindow.h"

#include "Game.h"
#include "Layout.h"

SaveWindow::SaveWindow(Layout* layout)
    : _layout(layout),
      _fpathText(utility::Constants::defaultFont),
      _result(),
      _choose(nullptr, {300, 10}, {60, 20}, utility::Constants::defaultFont,
              "Choose", sf::Color(120, 120, 120), sf::Color(100, 100, 100),
              sf::Color::Black),
      _recent1(nullptr, {10, 40}, {60, 20}, utility::Constants::defaultFont,
               "[1]", sf::Color(120, 120, 120), sf::Color(100, 100, 100),
               sf::Color::Black),
      _recent2(nullptr, {80, 40}, {60, 20}, utility::Constants::defaultFont,
               "[2]", sf::Color(120, 120, 120), sf::Color(100, 100, 100),
               sf::Color::Black),
      _recent3(nullptr, {150, 40}, {60, 20}, utility::Constants::defaultFont,
               "[3]", sf::Color(120, 120, 120), sf::Color(100, 100, 100),
               sf::Color::Black),
      _recent4(nullptr, {220, 40}, {60, 20}, utility::Constants::defaultFont,
               "[4]", sf::Color(120, 120, 120), sf::Color(100, 100, 100),
               sf::Color::Black),
      _recent5(nullptr, {290, 40}, {60, 20}, utility::Constants::defaultFont,
               "[5]", sf::Color(120, 120, 120), sf::Color(100, 100, 100),
               sf::Color::Black),
      _ok(nullptr, {290, 70}, {60, 20}, utility::Constants::defaultFont, "ok",
          sf::Color(120, 120, 120), sf::Color(100, 100, 100), sf::Color::Black),
      _cancel(nullptr, {10, 70}, {60, 20}, utility::Constants::defaultFont,
              "cancel", sf::Color(120, 120, 120), sf::Color(100, 100, 100),
              sf::Color::Black) {
  _choose.setOnclick([this](Game* gm) { this->onclickChoose(); });
  _recent1.setOnclick([this](Game* gm) { this->onclickRecent(0); });
  _recent2.setOnclick([this](Game* gm) { this->onclickRecent(1); });
  _recent3.setOnclick([this](Game* gm) { this->onclickRecent(2); });
  _recent4.setOnclick([this](Game* gm) { this->onclickRecent(3); });
  _recent5.setOnclick([this](Game* gm) { this->onclickRecent(4); });
  _ok.setOnclick([this](Game* gm) { this->onclickOk(); });
  _cancel.setOnclick([this](Game* gm) { this->onclickCancel(); });
  _background.setSize({360, 100});
  this->setOrigin(sf::Vector2f{_background.getSize().x, _background.getSize().y}/2.f);
}

void SaveWindow::setSize(sf::Vector2f& size) { this->setPosition(size / 2.f); }

void SaveWindow::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
  if (auto e = event.getIf<sf::Event::MouseButtonPressed>()) {
  }
}

void SaveWindow::update(sf::RenderWindow& rw) {
  auto pos = rw.getView().getSize() / 2.f;
  this->setPosition(pos);
  _choose.update(rw);
  _recent1.update(rw);
  _recent2.update(rw);
  _recent3.update(rw);
  _recent4.update(rw);
  _recent5.update(rw);
  _ok.update(rw);
  _cancel.update(rw);
}

void SaveWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_background, states);
  target.draw(_fpathText, states);
  _choose.draw(target, states);
  _recent1.draw(target, states);
  _recent2.draw(target, states);
  _recent3.draw(target, states);
  _recent4.draw(target, states);
  _recent5.draw(target, states);
  _ok.draw(target, states);
  _cancel.draw(target, states);
}

SaveWindowStatus SaveWindow::getStatus() const { return _status; }

std::filesystem::path SaveWindow::getResult() const { return _result; }

void SaveWindow::onclickChoose() {
  std::string result =
      pfd::select_folder("JAM - choose a save folder").result();
  if (!result.empty()) {
    _fpathText.setString(result);
    _result = std::filesystem::path(result);
  }
}

void SaveWindow::onclickRecent(size_t num) {
  _fpathText.setString(_recents.at(num).string());
  _result = _recents.at(num);
}

void SaveWindow::onclickOk() {
  if (std::filesystem::exists(_result)) {
    _status = SaveWindowStatus::EXIT_DONE;
  }
}

void SaveWindow::onclickCancel() { _status = SaveWindowStatus::EXIT_CANCEL; }
