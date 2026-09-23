#include "SaveWindow.h"

#include "Game.h"
#include "Layout.h"

SaveWindow::SaveWindow(Layout* layout)
    : _layout(layout),
      _fpathText(utility::Constants::defaultFont),
      _result(),
      _status(SaveWindowStatus::NORMAL),
      _choose(nullptr, {390, 10}, {80, 30}, utility::Constants::defaultFont,
              "Choose", utility::ColorPalette::functionalBtn, utility::ColorPalette::functionalBtnHover,
              sf::Color::Black),
      _ok(nullptr, {390, 110}, {80, 30}, utility::Constants::defaultFont, "ok",
          utility::ColorPalette::functionalBtn, utility::ColorPalette::functionalBtnHover, sf::Color::Black),
      _cancel(nullptr, {10, 110}, {80, 30}, utility::Constants::defaultFont,
              "cancel", utility::ColorPalette::destructiveBtn, utility::ColorPalette::destructiveBtnHover,
              sf::Color::Black) {
  _choose.setOnclick([this](Game* gm) { this->onclickChoose(); });
  _ok.setOnclick([this](Game* gm) { this->onclickOk(); });
  _cancel.setOnclick([this](Game* gm) { this->onclickCancel(); });
  _background.setSize({500, 150});
  _fpathText.setCharacterSize(20);
  _fpathText.setPosition({10, 10});
  _fpathText.setFillColor(sf::Color::Black);
  this->setOrigin(
      sf::Vector2f{_background.getSize().x, _background.getSize().y} / 2.f);
}

void SaveWindow::setSize(sf::Vector2f size) { this->setPosition(size / 2.f); }

void SaveWindow::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
  if (auto e = event.getIf<sf::Event::MouseButtonPressed>()) {
  }
}

void SaveWindow::update(sf::Vector2f mousePosition, bool enable) {
  mousePosition = getInverseTransform().transformPoint(mousePosition);
  _choose.update(mousePosition, enable);
  _ok.update(mousePosition, enable);
  _cancel.update(mousePosition, enable);
}

void SaveWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  target.draw(_background, states);
  target.draw(_fpathText, states);
  _choose.draw(target, states);
  _ok.draw(target, states);
  _cancel.draw(target, states);
}

SaveWindowStatus SaveWindow::getStatus() const { return _status; }

std::filesystem::path SaveWindow::getResult() const { return _result; }

void SaveWindow::onclickChoose() {
  std::string result =
      pfd::select_folder("JAM - choose a save folder").result();
  if (!result.empty()) {
    _fpathText.setString(utility::truncateText(_fpathText, result, 270));
    _result = std::filesystem::path(result);
  }
}

void SaveWindow::onclickRecent(size_t num) {
  _fpathText.setString(_recents.at(num).string());
  _result = _recents.at(num);
}

void SaveWindow::onclickOk() {
  if (std::filesystem::is_directory(_result)) {
    _status = SaveWindowStatus::EXIT_DONE;
  } else if (_result.empty()) {
    pfd::message("JAM", "Please enter a folder path!", pfd::choice::ok, pfd::icon::error);
  } else {
    pfd::message("JAM", "Invalid path!", pfd::choice::ok, pfd::icon::error);
  }
}

void SaveWindow::onclickCancel() { _status = SaveWindowStatus::EXIT_CANCEL; }
