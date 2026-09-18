#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <vector>

//#define DEBUG 1

namespace utility {

size_t uid();
static size_t _maxPhysicalID = 0;
void registerPhysicalID(size_t id);
size_t getNewPhysicalID();
void logErr(std::string msg);
void logWarn(std::string msg);
void log(std::string msg);
void exit(std::string msg = "Fatal error", int code=-1);

// x from [0, 1], returns [0, 1]
double smoothstep(double x);

// given a function p(t) of event occuring, and given that even has not occured up to t
// what is the chance event occurs during time interval [t, t+dt]
double cumlitivePtoSlice(double (*p)(double), double t, double dt);

template <class T>
bool isclose(T a, T b, T tol = 1e-9){
  return (a-b) > -tol && (a-b) < tol;
}

std::filesystem::path getSaveFileDirectory();
std::filesystem::path getSaveFilePath(const std::string& gameName, const std::string& fileName);

std::string truncateText(sf::Text &textObj, const std::string &str, float maxWidth);

std::vector<sf::Vector2f> getVerticies(sf::RectangleShape shape);
std::pair<float, float> projectVerticies(const std::vector<sf::Vector2f> &verticies, sf::Vector2f axis);
bool rectanglesIntersect(sf::RectangleShape a, sf::RectangleShape b);

namespace Constants{
  inline const sf::Font defaultFont{"assets/fonts/arial/ARIAL.TTF"};
  extern const float GREEN_PHASE_TIME_DEFAULT;
  extern const float YELLOW_PHASE_TIME_DEFAULT;
  extern const float INTERSESCTION_SIZE;
  extern const float INTERSECTION_SNAP_DIST;
  extern const float MIN_ROAD_DIST;
  extern const float TIMOUT_LIMIT_NO_MOVE_CAR;
  extern const float INTERSECTION_TRANSITION_LENGTH;
  extern const float ROAD_SELECT_SNAP_DIST;
}

};