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
    saveDirectory = std::filesystem::path(home) / "Library" / "Application Support" / folderName;
  }
#else
  const char* xdgData = std::getenv("XDG_DATA_HOME");
  if (xdgData) {
    saveDirectory = std::filesystem::path(xdgData) / folderName;
  } else {
    const char* home = std::getenv("HOME");
    if (home) {
      saveDirectory = std::filesystem::path(home) / ".local" / "share" / folderName;
    }
  }
#endif
  if(!saveDirectory.empty() && !std::filesystem::exists(saveDirectory)){
    std::filesystem::create_directory(saveDirectory);
  }
  return saveDirectory;
}

std::filesystem::path utility::getSaveFilePath(const std::string& gameName,
                                               const std::string& fileName) {
  return getSaveFileDirectory() / gameName / fileName;
}

const float utility::Constants::GREEN_PHASE_TIME_DEFAULT = 20;
const float utility::Constants::YELLOW_PHASE_TIME_DEFAULT = 5;
const float utility::Constants::INTERSESCTION_SIZE = 50.f;
const float utility::Constants::INTERSECTION_SNAP_DIST = 60.f;
const float utility::Constants::MIN_ROAD_DIST = 150.f;
const float utility::Constants::TIMOUT_LIMIT_NO_MOVE_CAR = 75.f;
const float utility::Constants::INTERSECTION_TRANSITION_LENGTH = 50.f;
const float utility::Constants::ROAD_SELECT_SNAP_DIST = 50.f;