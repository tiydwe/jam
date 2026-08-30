#pragma once

#include <string>

#define DEBUG 1

namespace utility {

size_t uid();
void logErr(std::string msg);
void logWarn(std::string msg);
void log(std::string msg);
void exit(std::string msg = "Fatal error", int code=-1);

};