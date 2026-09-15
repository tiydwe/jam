#include "EditorWindow.h"


#include "utility.h"
#include "Button.h"

#include "Game.h"
#include "Layout.h"

void EditorWindow::makeRoad(sf::Vector2f pos2, std::string datapath) {
  auto start = _l->findClosestIntersection(_lastClickedPos);
  auto end = _l->findClosestIntersection(pos2);
  if(start.first == nullptr || start.second > utility::Constants::INTERSECTION_SNAP_DIST){
    // create new
    start.first = _l->createIntersection(_lastClickedPos);
  }
  if(end.first == nullptr || end.second > utility::Constants::INTERSECTION_SNAP_DIST){
    // create new
    end.first = _l->createIntersection(pos2);
  }
  _l->createRoad(*start.first, *end.first, datapath, false);
}
