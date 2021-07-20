#include <string>
#include <sstream>

#include "format.h"
#define HOUR 3600
#define MIN 60
using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hour, minute, second;
    hour = seconds / HOUR;
    minute = seconds % HOUR / MIN;
    second = seconds % HOUR % MIN;
    std::stringstream tstr;
    tstr << hour << ":" << minute << ":" << second << "\n";
    return tstr.str();
}