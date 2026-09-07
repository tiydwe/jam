#pragma once

#include <SFML/Graphics.hpp>
#include "Layout.h"
#include "Button.h"

#include <memory>
#include <utility>

class Game;

enum class ActionType{
  NONE, DRAW_ROAD
};

std::string getDatapathFromActionType(ActionType type);

class EditorWindow : public sf::Drawable, sf::Transformable{
 public:
  EditorWindow(std::unique_ptr<Layout> l, Game* game, sf::Vector2u windowSize);

  void handleEvent(const sf::Event& event, sf::RenderWindow& window);

  void updateWindowSize(sf::Vector2f newSize);

  void update(sf::RenderWindow &rw);

  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;
  
  Layout* getLayout() const {return _l.get();}

 private:
  void onclickSimulate(Game* game);
  void onclickCreateRoad(Game* game);
  void makeRoad(sf::Vector2f pos2, std::string datapath);
  std::unique_ptr<Layout> _l;

  Game* _game;

  sf::View _uiview;
  sf::View _worldview;

  bool _isDragging = false;
  sf::Vector2i _oldMousePos;
  double _zoomLevel = 1.0;

  sf::RectangleShape _topbar;

  Button _simulateButton;

  Button _placeRoadButton;
  
  sf::Vector2f _lastClickedPos;
  size_t _clickedCtr = 0;
  ActionType _currentAction = ActionType::NONE;
};