#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <utility>

#include "Button.h"
#include "Layout.h"
#include "SaveWindow.h"

class Game;

enum class ActionType {
  NONE,
  DRAW_ROAD_4LANE,
  DRAW_ROAD_2LANE,
  DRAW_ROAD_1_1LANE,
  DRAW_ROAD_1_2LANE,
  REMOVE_ROAD
};

std::string getDatapathFromActionType(ActionType type);

class EditorWindow : public sf::Drawable, sf::Transformable {
 public:
  EditorWindow(std::unique_ptr<Layout> l, Game* game, sf::Vector2u windowSize);

  void handleEvent(const sf::Event& event, sf::RenderWindow& window);

  void updateWindowSize(sf::Vector2f newSize);

  void update(sf::Vector2f mousePosition);

  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;

  Layout* getLayout() const { return _l.get(); }

 private:
 bool isDrawRoad(ActionType tp) const;
  void onclickSimulate(Game* game);
  void onclickCreateRoad(Game* game, ActionType at);
  void onclickDemolishRoad(Game* game);
  void onclickSaveGame(Game* game);
  void makeRoad(sf::Vector2f pos2, std::string datapath);
  // will not do snapping
  std::pair<bool, sf::RectangleShape> isRoadValid(sf::Vector2f start,
                                                  sf::Vector2f end,
                                                  std::string roadPath) const;
  sf::Vector2f getSnappedPos(sf::Vector2f pos) const;
  std::unique_ptr<Layout> _l;

  Game* _game;

  sf::View _uiview;
  sf::View _worldview;

  bool _isDragging = false;
  sf::Vector2i _oldMousePos;
  double _zoomLevel = 0.7;

  sf::RectangleShape _topbar;

  Button _simulateButton;

  Button _placeRoad1Way1LaneButton;
  Button _placeRoad1Way2LaneButton;
  Button _placeRoad2LaneButton;
  Button _placeRoad4LaneButton;
  Button _removeRoadButton;
  Button _saveButton;

  sf::Vector2f _lastClickedPos;
  size_t _clickedCtr = 0;
  ActionType _currentAction = ActionType::NONE;
  sf::Vector2f _mouseWorldPos;

  std::unique_ptr<SaveWindow> _saveWindow;
};