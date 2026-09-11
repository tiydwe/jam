#include "utility.h"

#include <iostream>

size_t utility::uid() {
  static size_t curr = 0;
  return curr++;
}

void utility::registerPhysicalID(size_t id) {
  _maxPhysicalID = std::max(_maxPhysicalID, id);
}

size_t utility::getNewPhysicalID() {
  return ++_maxPhysicalID;
}

void utility::logErr(std::string msg) {
  std::cerr << "ERROR: " << msg << "\n";
}

void utility::logWarn(std::string msg) {
  std::cerr << "WARNING: " << msg << "\n";
}

void utility::log(std::string msg) {
  std::cout << "INFO: " << msg << "\n";
}

void utility::exit(std::string msg, int code){
  std::cerr << "EXITING DUE TO: " << msg << "\n";
  throw msg;
  //std::exit(code);
}

double utility::smoothstep(double x) {
  x = std::min(1.0, std::max(0.0, x));
  return 6*x*x*x*x*x - 15*x*x*x*x + 10*x*x*x;
}

double utility::cumlitivePtoSlice(double (*p)(double), double t, double dt) {
  return 1.0-(1.0-p(t+dt))/(1.0-p(t));
}

const float utility::Constants::GREEN_PHASE_TIME_DEFAULT = 20;
const float utility::Constants::YELLOW_PHASE_TIME_DEFAULT = 5;
const float utility::Constants::INTERSESCTION_SIZE = 50.f;
const float utility::Constants::SNAP_DIST = 60.f;
const float utility::Constants::MIN_ROAD_DIST = 125.f;
const float utility::Constants::TIMOUT_LIMIT_NO_MOVE_CAR = 75.f;