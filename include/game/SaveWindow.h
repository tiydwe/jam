#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <string>
#include <vector>

#include "Button.h"
#include "portable-file-dialogs.h"
#include "utility.h"

class Layout;

enum class SaveWindowStatus{
  NORMAL, EXIT_DONE, EXIT_CANCEL
};

class SaveWindow : public sf::Drawable, sf::Transformable {
 public:
  SaveWindow(Layout* layout);
  void setSize(sf::Vector2f size);
  void handleEvent(const sf::Event& event, sf::RenderWindow& window);
  void update(sf::Vector2f mousePosition, bool enable);
  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;
  SaveWindowStatus getStatus() const;
  std::filesystem::path getResult() const;
 private:
  void onclickChoose();
  void onclickRecent(size_t num);
  void onclickOk();
  void onclickCancel();
  SaveWindowStatus _status;
  Layout* _layout;
  sf::RectangleShape _background;
  sf::Text _fpathText;
  std::filesystem::path _result;

  Button _choose;

  Button _ok;
  Button _cancel;

  std::vector<std::filesystem::path> _recents;
};