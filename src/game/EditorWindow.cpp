#include "EditorWindow.h"

#include <fstream>
#include <istream>
#include <sstream>
#include <string>

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
                       sf::Color::Green, sf::Color::Blue, sf::Color::Red),
      _removeRoadButton(g, sf::Vector2f(500, 10), sf::Vector2f(100, 40),
                        utility::Constants::defaultFont, "DEMOLISH",
                        sf::Color::Green, sf::Color::Blue, sf::Color::Red),
      _saveButton(g, sf::Vector2f(620, 10), sf::Vector2f(100, 40),
                  utility::Constants::defaultFont, "SAVE", sf::Color::Green,
                  sf::Color::Blue, sf::Color::Red),
      _saveWindow(nullptr) {
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
  _removeRoadButton.setOnclick(
      [&](Game* game) { this->onclickDemolishRoad(game); });
  _saveButton.setOnclick([this](Game* game) { this->onclickSaveGame(game); });
}

void EditorWindow::handleEvent(const sf::Event& event,
                               sf::RenderWindow& window) {
  if (_saveWindow.get() != nullptr) {
    _saveWindow->handleEvent(event, window);
    return;
  }
  auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
  if (!_topbar.getGlobalBounds().contains(pos)) {
    if (auto mbpIf = event.getIf<sf::Event::MouseButtonPressed>()) {
      if (mbpIf->button == sf::Mouse::Button::Right) {
        _isDragging = true;
        _oldMousePos = sf::Mouse::getPosition(window);
      }
      if (mbpIf->button == sf::Mouse::Button::Left) {
        switch (_currentAction) {
          case ActionType::DRAW_ROAD: {
            if (_clickedCtr == 0) {
              // first click
              ++_clickedCtr;
              sf::View old = window.getView();
              window.setView(_worldview);
              _lastClickedPos =
                  window.mapPixelToCoords(sf::Mouse::getPosition(window));
              window.setView(old);
            } else {
              auto irv = isRoadValid(getSnappedPos(_mouseWorldPos), getSnappedPos(_lastClickedPos),
                                     getDatapathFromActionType(_currentAction));
              if (irv.first) {
                sf::View old = window.getView();
                window.setView(_worldview);
                auto pos =
                    window.mapPixelToCoords(sf::Mouse::getPosition(window));
                this->makeRoad(pos, getDatapathFromActionType(_currentAction));
                window.setView(old);
                _clickedCtr = 0;
              }
            }
            break;
          }
          case ActionType::REMOVE_ROAD: {
            // already checks for locked
            auto road = _l->findClosestRoadUnlocked(_mouseWorldPos);
            if (road.first != nullptr &&
                road.second < utility::Constants::ROAD_SELECT_SNAP_DIST) {
              _l->removeRoad(road.first->getID());
            }
            break;
          }
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
  if (auto kps = event.getIf<sf::Event::KeyPressed>()) {
    if (kps->code == sf::Keyboard::Key::Escape) {
      _clickedCtr = 0;
    }
  }
}

void EditorWindow::onclickSimulate(Game* game) {
  _clickedCtr = 0;
  game->beginSimulation();
}

void EditorWindow::onclickCreateRoad(Game* game) {
  _clickedCtr = 0;
  _currentAction = ActionType::DRAW_ROAD;
}

void EditorWindow::onclickDemolishRoad(Game* game) {
  _clickedCtr = 0;
  _currentAction = ActionType::REMOVE_ROAD;
}

void EditorWindow::onclickSaveGame(Game* game) {
  _saveWindow = std::make_unique<SaveWindow>(_l.get());
  _saveWindow->setSize({(float)game->getMainWindow()->getSize().x,
                        (float)game->getMainWindow()->getSize().y});
}

void EditorWindow::updateWindowSize(sf::Vector2f newSize) {
  if (_saveWindow.get() != nullptr) {
    _saveWindow->setSize(newSize);
  }
  _worldview.setSize(newSize);
  _uiview.setSize(newSize);
  _uiview.setCenter({newSize.x / 2, newSize.y / 2});
  _topbar.setSize({newSize.x, 60.f});
}

void EditorWindow::update(sf::Vector2f mousePosition) {
  _simulateButton.update(mousePosition);
  _placeRoadButton.update(mousePosition);
  _removeRoadButton.update(mousePosition);
  _saveButton.update(mousePosition);
  if (_saveWindow.get() != nullptr) {
    _saveWindow->update(mousePosition);
    switch (_saveWindow->getStatus()) {
      case SaveWindowStatus::EXIT_CANCEL:
        _saveWindow.reset(nullptr);
        break;
      case SaveWindowStatus::EXIT_DONE:
        _l->saveToFile(_saveWindow->getResult());
        _saveWindow.reset(nullptr);
        break;

      default:
        break;
    }
  }
}

void EditorWindow::draw(sf::RenderTarget& target,
                        sf::RenderStates states) const {
  sf::View origional = target.getView();
  target.setView(_worldview);
  if (_saveWindow.get() != nullptr) {
    _saveWindow->draw(target, states);
    return;
  }
  _l->draw(target, states);
  if (_clickedCtr == 1) {
    if (_currentAction == ActionType::DRAW_ROAD) {
      sf::Vector2f pos = getSnappedPos(_mouseWorldPos);
      sf::Vector2f direction = getSnappedPos(_lastClickedPos) - pos;
      float width = 30.f;
      float length =
          std::sqrt(direction.x * direction.x + direction.y * direction.y);
      sf::RectangleShape rectangle(sf::Vector2f(length, width));

      rectangle.setOrigin({0.f, width / 2.0f});

      rectangle.setPosition(pos);

      float angle = std::atan2(direction.y, direction.x);
      rectangle.setRotation(sf::radians(angle));
      auto irv = isRoadValid(pos, pos + direction,
                             getDatapathFromActionType(_currentAction));
      if (irv.first) {
        rectangle.setFillColor(sf::Color::Blue);
      } else {
        // sf::RectangleShape rect()
        rectangle.setFillColor(sf::Color::Red);
        irv.second.setFillColor(sf::Color(255, 0, 0, 32));
        target.draw(irv.second);
      }
      target.draw(rectangle);
    }
  }
  if (_currentAction == ActionType::REMOVE_ROAD) {
    auto road = _l->findClosestRoadUnlocked(_mouseWorldPos);
    if (road.first != nullptr &&
        road.second < utility::Constants::ROAD_SELECT_SNAP_DIST) {
      auto start = road.first->getStart();
      auto end = road.first->getEnd();
      float width = road.first->getRoadAsset()->leftCenterOffset.back() +
                    road.first->getRoadAsset()->rightCenterOffset.back();
      sf::RectangleShape rectangle(sf::Vector2f{(start - end).length(), width});
      rectangle.setOrigin({0.f, width / 2});
      rectangle.setRotation((end - start).angle());
      rectangle.setPosition(start);
      rectangle.setFillColor(sf::Color(255, 0, 0, 32));
      target.draw(rectangle);
    }
  }
  target.setView(_uiview);
  target.draw(_topbar, states);

  target.draw(_simulateButton, states);
  target.draw(_placeRoadButton, states);
  target.draw(_removeRoadButton, states);
  target.draw(_saveButton, states);

  target.setView(origional);
}

std::pair<bool, sf::RectangleShape> EditorWindow::isRoadValid(
    sf::Vector2f start, sf::Vector2f end, std::string roadPath) const {
  if ((start - end).length() < utility::Constants::MIN_ROAD_DIST) {
    return {false, sf::RectangleShape{}};
  }
  std::ifstream file(roadPath);
  if (!file.is_open()) {
    utility::logErr("EditorWindow::isRoadValid - could not open road " +
                    roadPath);
    return {false, sf::RectangleShape{}};
  }
  std::string tmp;
  std::getline(file, tmp);
  std::getline(file, tmp);
  std::getline(file, tmp);
  std::getline(file, tmp);
  sf::Image im;
  if (im.loadFromFile(tmp)) {
    auto height = im.getSize().y;
    auto delta = end - start;
    // fudge it a bit to make placement easier
    auto adjStart = start + delta.normalized() *
                                (utility::Constants::INTERSESCTION_SIZE);
    auto adjEnd = end - delta.normalized() *
                            (utility::Constants::INTERSESCTION_SIZE);
    sf::RectangleShape hitbox({(adjEnd - adjStart).length(), height});
    hitbox.setOrigin({0.f, hitbox.getSize().y / 2.f});
    hitbox.setRotation((adjEnd - adjStart).angle());
    hitbox.setPosition(adjStart);
    auto hb = _l->checkHitbox(hitbox);
    if (hb.first) {
      return {false, hb.second};
    }
  }
  return {true, sf::RectangleShape{}};
}

sf::Vector2f EditorWindow::getSnappedPos(sf::Vector2f pos) const {
  auto x = _l->findClosestIntersection(pos);
  if (x.first == nullptr) {
    return pos;
  } else {
    if ((x.first->getPos() - pos).length() >
        utility::Constants::INTERSECTION_SNAP_DIST *
            std::min(_zoomLevel, 1.0)) {
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
