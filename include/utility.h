#pragma once

#include <string>

#define DEBUG 1

namespace utility {

size_t uid();
void logErr(std::string msg);
void logWarn(std::string msg);
void log(std::string msg);
void exit(std::string msg = "Fatal error", int code=-1);

template <class T>
bool isclose(T a, T b, T tol = 1e-9){
  return (a-b) > -tol && (a-b) < tol;
}

};