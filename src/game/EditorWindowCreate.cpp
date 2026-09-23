#include "EditorWindow.h"


#include "utility.h"
#include "Button.h"

#include "Game.h"
#include "Layout.h"

void EditorWindow::makeRoad(sf::Vector2f pos2, std::string datapath) {
  auto start = _lastip;
  auto end = _l->findClosestIntersection(pos2);
  // lastip is pre adjusted
  if(start.first == nullptr || start.second > utility::Constants::INTERSECTION_SNAP_DIST){
    // create new
    start.first = _l->createIntersection(_lastClickedPos);
  }
  if(end.first == nullptr || end.second > utility::Constants::INTERSECTION_SNAP_DIST * std::min(_zoomLevel,1.0)){
    // create new
    end.first = _l->createIntersection(pos2);
  }
  _l->createRoad(*start.first, *end.first, datapath, false);
}
