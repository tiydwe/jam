#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>

#include "Button.h"
#include "Simulation.h"
#include "utility.h"

enum class LevelStatus { COMPLETE, INCOMPLETE };

enum class LevelSelectWindowStatus { NORMAL, EXIT_BACK, EXIT_DONE };

class LevelSelectWindow : public sf::Drawable, sf::Transformable {
 public:
  // levels directory
  LevelSelectWindow(std::filesystem::path levelsDir, sf::Vector2f windowSize,
                    Game* game);
  void updateWindowSize(sf::Vector2f newSize);
  void update(sf::Vector2f mousePos);
  // returns the main.dat path
  std::filesystem::path getChoosenLevel() const;
  virtual void draw(sf::RenderTarget& target,
                    sf::RenderStates states) const override;

 private:
  void onclickLevel(std::filesystem::path levelPath);
  void onclickBack();
  void createButtons(sf::Vector2f windowSize);
  Game* _game;
  std::vector<std::pair<std::filesystem::path, LevelStatus>> _levels;
  std::vector<std::unique_ptr<Button>> _levelButtons;
  Button _back;
  std::filesystem::path _choosenLevel;
  LevelSelectWindowStatus _status = LevelSelectWindowStatus::NORMAL;
  sf::RectangleShape _contentBox;
  sf::Text _title;
};