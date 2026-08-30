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

