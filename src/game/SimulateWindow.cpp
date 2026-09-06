#include "SimulateWindow.h"
#include "Game.h"
#include "utility.h"

SimulateWindow::SimulateWindow(Layout* l, Game* g,std::string filename, sf::Vector2u windowSize)
    : _l(l),
      _g(g),
      _slow(g, sf::Vector2f(10, 10), sf::Vector2f(100, 40),
                      utility::Constants::defaultFont, "Slow",
                      sf::Color::Green, sf::Color::Blue, sf::Color::Red),
      _mid(g, sf::Vector2f(220, 10), sf::Vector2f(100, 40),
                      utility::Constants::defaultFont, "Normal",
                      sf::Color::Green, sf::Color::Blue, sf::Color::Red),
      _fast(g, sf::Vector2f(330, 10), sf::Vector2f(100, 40),
                      utility::Constants::defaultFont, "Fast",
                      sf::Color::Green, sf::Color::Blue, sf::Color::Red) {
  _s = std::make_unique<Simulation>(l, filename);
  _worldview.setSize({(float)windowSize.x, (float)windowSize.y});
  _worldview.setCenter({windowSize.x / 2.f, windowSize.y / 2.f});
  _uiview.setSize({(float)windowSize.x, (float)windowSize.y});
  _uiview.setCenter({windowSize.x / 2.f, windowSize.y / 2.f});

  _topbar.setSize({(float)windowSize.x, 60.2f});
  _topbar.setFillColor(sf::Color::Blue);
  _topbar.setPosition({0.f, 0.f});
  
  _slow.setOnclick([&](Game* gm){_timeMultiplier = 5.0;});
  _mid.setOnclick([&](Game* gm){_timeMultiplier = 8.0;});
  _fast.setOnclick([&](Game* gm){_timeMultiplier = 12.0;});
  
}

void SimulateWindow::handleEvent(const sf::Event& event,
                               const sf::RenderWindow& window) {
  if (auto mbpIf = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mbpIf->button == sf::Mouse::Button::Right) {
      _isDragging = true;
      _oldMousePos = sf::Mouse::getPosition(window);
    }
  }
  if (auto mbrIf = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mbrIf->button == sf::Mouse::Button::Right) {
      _isDragging = false;
    }
  }
  if (event.is<sf::Event::MouseMoved>()) {
    if (_isDragging) {
      sf::Vector2i newpos = sf::Mouse::getPosition(window);
      sf::Vector2f change = window.mapPixelToCoords(_oldMousePos, _worldview) -
                            window.mapPixelToCoords(newpos, _worldview);
      _worldview.move(change);
      _oldMousePos = newpos;
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
}

void SimulateWindow::updateWindowSize(sf::Vector2f newSize) {
  _worldview.setSize(newSize);
  _uiview.setSize(newSize);
  _uiview.setCenter({newSize.x / 2, newSize.y / 2});
  _topbar.setSize({newSize.x, 60.f});
}

void SimulateWindow::step(double trueDt) {
  _s->step(trueDt * _timeMultiplier);
}

void SimulateWindow::update(sf::RenderWindow& rw) {
  _slow.update(rw);
  _mid.update(rw);
  _fast.update(rw);
}

void SimulateWindow::draw(sf::RenderTarget& target,
                        sf::RenderStates states) const {
  sf::View origional = target.getView();
  target.setView(_worldview);
  _l->draw(target, states);
  _s->draw(target, states);
  target.setView(_uiview);
  target.draw(_topbar, states);
  target.draw(_slow, states);
  target.draw(_mid, states);
  target.draw(_fast, states);

  target.setView(origional);
}