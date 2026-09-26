#include "utility.h"

#include <cstdlib>
#include <iostream>

size_t utility::uid() {
  static size_t curr = 0;
  return curr++;
}

void utility::registerPhysicalID(size_t id) {
  _maxPhysicalID = std::max(_maxPhysicalID, id);
}

size_t utility::getNewPhysicalID() { return ++_maxPhysicalID; }

void utility::logErr(std::string msg) { std::cerr << "ERROR: " << msg << "\n"; }

void utility::logWarn(std::string msg) {
  std::cerr << "WARNING: " << msg << "\n";
}

void utility::log(std::string msg) { std::cout << "INFO: " << msg << "\n"; }

void utility::exit(std::string msg, int code) {
  std::cerr << "EXITING DUE TO: " << msg << "\n";
  throw msg;
  // std::exit(code);
}

double utility::smoothstep(double x) {
  x = std::min(1.0, std::max(0.0, x));
  return 6 * x * x * x * x * x - 15 * x * x * x * x + 10 * x * x * x;
}

double utility::cumlitivePtoSlice(double (*p)(double), double t, double dt) {
  return 1.0 - (1.0 - p(t + dt)) / (1.0 - p(t));
}

std::filesystem::path utility::getSaveFileDirectory() {
  std::filesystem::path saveDirectory;
  std::string folderName = "jam_game";
#if defined(_WIN32)
  const char* appData = std::getenv("APPDATA");
  if (appData) {
    saveDirectory = std::filesystem::path(appData) / folderName;
  } else {
    saveDirectory = std::filesystem::current_path() / "saves";
  }
#elif defined(__APPLE__)
  const char* home = std::getenv("HOME");
  if (home) {
    saveDirectory = std::filesystem::path(home) / "Library" /
                    "Application Support" / folderName;
  }
#else
  const char* xdgData = std::getenv("XDG_DATA_HOME");
  if (xdgData) {
    saveDirectory = std::filesystem::path(xdgData) / folderName;
  } else {
    const char* home = std::getenv("HOME");
    if (home) {
      saveDirectory =
          std::filesystem::path(home) / ".local" / "share" / folderName;
    }
  }
#endif
  if (!saveDirectory.empty() && !std::filesystem::exists(saveDirectory)) {
    std::filesystem::create_directory(saveDirectory);
  }
  return saveDirectory;
}

std::filesystem::path utility::getSaveFilePath(const std::string& gameName,
                                               const std::string& fileName) {
  return getSaveFileDirectory() / gameName / fileName;
}

std::string utility::truncateText(sf::Text& textObj, const std::string& str,
                                  float maxWidth) {
  textObj.setString(str);
  if (textObj.getLocalBounds().size.x <= maxWidth) {
    return str;
  }

  std::string ellipsis = "...";
  textObj.setString(ellipsis);
  float ellipsisWidth = textObj.getLocalBounds().size.x;

  float targetWidth = maxWidth - ellipsisWidth;

  std::string truncated = "";
  for (size_t i = 0; i < str.size(); ++i) {
    textObj.setString(truncated + str[i]);
    if (textObj.getLocalBounds().size.x > targetWidth) {
      break;
    }
    truncated += str[i];
  }

  return truncated + ellipsis;
}

std::vector<sf::Vector2f> utility::getVerticies(sf::RectangleShape shape) {
  std::vector<sf::Vector2f> res(4);
  for (int i = 0; i < 4; ++i) {
    res[i] = shape.getTransform().transformPoint(shape.getPoint(i));
  }
  return res;
}

std::pair<float, float> utility::projectVerticies(
    const std::vector<sf::Vector2f>& verticies, sf::Vector2f axis) {
  std::pair<float, float> res{
      verticies[0].x * axis.x + verticies[0].y * axis.y,
      verticies[0].x * axis.x + verticies[0].y * axis.y};
  for (int i = 1; i < verticies.size(); ++i) {
    auto proj = verticies[i].x * axis.x + verticies[i].y * axis.y;
    res.first = std::min(res.first, proj);
    res.second = std::max(res.second, proj);
  }
  return res;
}

bool utility::rectanglesIntersect(sf::RectangleShape a, sf::RectangleShape b) {
  auto va = getVerticies(a);
  auto vb = getVerticies(b);
  std::vector<sf::Vector2f> axes;
  for (int i = 0; i < 4; ++i) {
    auto delta = va[(i == 3 ? 0 : i + 1)] - va[i];
    axes.push_back(sf::Vector2f{-delta.y, delta.x}.normalized());
  }
  for (int i = 0; i < 4; ++i) {
    auto delta = vb[(i == 3 ? 0 : i + 1)] - vb[i];
    axes.push_back(sf::Vector2f{-delta.y, delta.x}.normalized());
  }
  for (auto ax : axes) {
    auto pa = projectVerticies(va, ax);
    auto pb = projectVerticies(vb, ax);
    if (pa.second <= pb.first || pb.second <= pa.first) {
      return false;
    }
  }
  return true;
}

const sf::Font utility::Constants::defaultFont{"assets/fonts/montserrat-latin-700-normal.ttf"};

const sf::Texture utility::Constants::noRouteIcon{"assets/textures/cars/noroute.png"};
const sf::Texture utility::Constants::titleScreenBackground{"assets/images/gameimage1.png"};
const float utility::Constants::GREEN_PHASE_TIME_DEFAULT = 50;
const float utility::Constants::YELLOW_PHASE_TIME_DEFAULT = 5;
const float utility::Constants::INTERSESCTION_SIZE = 50.f;
const float utility::Constants::INTERSECTION_SNAP_DIST =
    utility::Constants::INTERSESCTION_SIZE * 1.6;
const float utility::Constants::MIN_ROAD_DIST =
    2 * utility::Constants::INTERSESCTION_SIZE + 32;
const float utility::Constants::TIMOUT_LIMIT_NO_MOVE_CAR = 300.f;
const float utility::Constants::INTERSECTION_TRANSITION_LENGTH = 90.f;
const float utility::Constants::ROAD_SELECT_SNAP_DIST = 50.f;
const sf::Color utility::Constants::BACKGROUND_COLOR =
    sf::Color(52, 69, 36);
const float utility::Constants::CAR_MARGIN = 40.f;


const sf::Color utility::ColorPalette::functionalBtn = sf::Color(43, 133, 165);
const sf::Color utility::ColorPalette::functionalBtnHover = sf::Color(42, 107, 130);
const sf::Color utility::ColorPalette::destructiveBtn = sf::Color(199, 40, 52);
const sf::Color utility::ColorPalette::destructiveBtnHover = sf::Color(156, 31, 41);
const sf::Color utility::ColorPalette::editorBtn = sf::Color(121, 145, 23);
const sf::Color utility::ColorPalette::editorBtnHover = sf::Color(92, 110, 19);
const sf::Color utility::ColorPalette::uiBackgroundColor = sf::Color(2, 9, 59);