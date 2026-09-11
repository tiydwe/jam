#include "EditorWindow.h"

#include "Button.h"
#include "Game.h"
#include "Layout.h"
#include "utility.h"

EditorWindow::EditorWindow(std::unique_ptr<Layout> l, Game* g,
                           sf::Vector2u windowSize)
    : _l(std::move(l)),
      _game(g),
      _simulateButton(g, sf::Vector2f(10, 10), sf::Vector2f(100, 40),
                      utility::Constants::defaultFont, "Simulate",
                      sf::Color::Green, sf::Color::Blue, sf::Color::Red),
      _placeRoadButton(g, sf::Vector2f(120, 10), sf::Vector2f(100, 40),
                       utility::Constants::defaultFont, "ROAD",
                       sf::Color::Green, sf::Color::Blue, sf::Color::Red) {
  _worldview.setSize({(float)windowSize.x, (float)windowSize.y});
  _worldview.setCenter({windowSize.x / 2.f, windowSize.y / 2.f});
  _uiview.setSize({(float)windowSize.x, (float)windowSize.y});
  _uiview.setCenter({windowSize.x / 2.f, windowSize.y / 2.f});

  _topbar.setSize({(float)windowSize.x, 60.2f});
  _topbar.setFillColor(sf::Color::Yellow);
  _topbar.setPosition({0.f, 0.f});

  _simulateButton.setOnclick([&](Game* game) { this->onclickSimulate(game); });
  _placeRoadButton.setOnclick(
      [&](Game* game) { this->onclickCreateRoad(game); });
}

void EditorWindow::handleEvent(const sf::Event& event,
                               sf::RenderWindow& window) {
  auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
  if (!_topbar.getGlobalBounds().contains(pos)) {
    if (auto mbpIf = event.getIf<sf::Event::MouseButtonPressed>()) {
      if (mbpIf->button == sf::Mouse::Button::Right) {
        _isDragging = true;
        _oldMousePos = sf::Mouse::getPosition(window);
      }
      if (mbpIf->button == sf::Mouse::Button::Left) {
        switch (_currentAction) {
          case ActionType::DRAW_ROAD:
            if (_clickedCtr == 0) {
              // first click
              ++_clickedCtr;
              sf::View old = window.getView();
              window.setView(_worldview);
              _lastClickedPos =
                  window.mapPixelToCoords(sf::Mouse::getPosition(window));
              window.setView(old);
            } else {
              if (isRoadValid(_mouseWorldPos,
                              _lastClickedPos)) {
                sf::View old = window.getView();
                window.setView(_worldview);
                auto pos =
                    window.mapPixelToCoords(sf::Mouse::getPosition(window));
                this->makeRoad(
                    pos, getDatapathFromActionType(ActionType::DRAW_ROAD));
                window.setView(old);
                _clickedCtr = 0;
              }
            }
            break;

          default:
            break;
        }
      }
    }
  }
  if (auto mbrIf = event.getIf<sf::Event::MouseButtonReleased>()) {
    if (mbrIf->button == sf::Mouse::Button::Right) {
      _isDragging = false;
    }
  }
  if (event.is<sf::Event::MouseMoved>()) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    _mouseWorldPos = window.mapPixelToCoords(mousePos, _worldview);

    if (_isDragging) {
      sf::Vector2f change = window.mapPixelToCoords(_oldMousePos, _worldview) -
                            window.mapPixelToCoords(mousePos, _worldview);
      _worldview.move(change);
      _oldMousePos = mousePos;
    }
  }
  if (auto mws = event.getIf<sf::Event::MouseWheelScrolled>()) {
    if (mws->wheel == sf::Mouse::Wheel::Vertical) {
      double zoomFactor = (mws->delta > 0) ? 0.9 : 1.1;

      // clip max zoom
      if ((_zoomLevel > 0.3 && zoomFactor < 1.0) ||
          (_zoomLevel < 3.0 && zoomFactor > 1.0)) {
        _worldview.zoom(zoomFactor);
        _zoomLevel *= zoomFactor;
      }
    }
  }
  if(auto kps = event.getIf<sf::Event::KeyPressed>()){
    if(kps->code == sf::Keyboard::Key::Escape){
      _clickedCtr = 0;
    }
  }
}

void EditorWindow::onclickSimulate(Game* game) { game->beginSimulation(); }

void EditorWindow::onclickCreateRoad(Game* game) {
  _clickedCtr = 0;
  _currentAction = ActionType::DRAW_ROAD;
}

void EditorWindow::updateWindowSize(sf::Vector2f newSize) {
  _worldview.setSize(newSize);
  _uiview.setSize(newSize);
  _uiview.setCenter({newSize.x / 2, newSize.y / 2});
  _topbar.setSize({newSize.x, 60.f});
}

void EditorWindow::update(sf::RenderWindow& rw) {
  _simulateButton.update(rw);
  _placeRoadButton.update(rw);
}

void EditorWindow::draw(sf::RenderTarget& target,
                        sf::RenderStates states) const {
  sf::View origional = target.getView();
  target.setView(_worldview);
  _l->draw(target, states);
  if (_clickedCtr == 1) {
    if (_currentAction == ActionType::DRAW_ROAD) {
      sf::Vector2f pos = getSnappedPos(_mouseWorldPos);
      sf::Vector2f direction = _lastClickedPos - pos;
      float width = 30.f;
      float length =
          std::sqrt(direction.x * direction.x + direction.y * direction.y);
      sf::RectangleShape rectangle(sf::Vector2f(length, width));

      rectangle.setOrigin({0.f, width / 2.0f});

      rectangle.setPosition(pos);

      float angle = std::atan2(direction.y, direction.x);
      rectangle.setRotation(sf::radians(angle));
      if (isRoadValid(pos, pos + direction)) {
        rectangle.setFillColor(sf::Color::Blue);
      } else {
        rectangle.setFillColor(sf::Color::Red);
      }
      target.draw(rectangle);
    }
  }
  target.setView(_uiview);
  target.draw(_topbar, states);

  target.draw(_simulateButton, states);
  target.draw(_placeRoadButton, states);

  target.setView(origional);
}

bool EditorWindow::isRoadValid(sf::Vector2f start, sf::Vector2f end) const {
  if ((start - end).length() < utility::Constants::MIN_ROAD_DIST) {
    return false;
  }
  return true;
}

sf::Vector2f EditorWindow::getSnappedPos(sf::Vector2f pos) const {
  auto x = _l->findClosest(pos);
  if (x.first == nullptr) {
    return pos;
  } else {
    if ((x.first->getPos() - pos).length() > utility::Constants::SNAP_DIST) {
      return pos;
    } else {
      return x.first->getPos();
    }
  }
}

std::string getDatapathFromActionType(ActionType type) {
  switch (type) {
    case ActionType::DRAW_ROAD:
      return "assets/data/roads/generic4lane.dat";

    default:
      return "";
  }
}
