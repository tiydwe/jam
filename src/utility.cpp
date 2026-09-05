#include "utility.h"

#include <iostream>

size_t utility::uid() {
  static size_t curr = 0;
  return curr++;
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

