#pragma once

#include <string>
#include <SFML/Graphics.hpp>

//#define DEBUG 1

#define GREEN_PHASE_TIME_DEFAULT 20
#define YELLOW_PHASE_TIME_DEFAULT 5

#define INTERSESCTION_SIZE 30.f

#define SNAP_DIST 50.f

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

namespace Constants{
  inline const sf::Font defaultFont{"assets/fonts/arial/ARIAL.TTF"};
}

};