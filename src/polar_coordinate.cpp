#include <raylib-cpp.hpp>
#include "polar_coordinate.h"

PolarCoordinate::PolarCoordinate(float mag, float the) : magnitude(mag), theta(the) {}

Vector2 PolarCoordinate::to_cartesian(Vector2 origin) {
    return {origin.x + magnitude*cos(theta), origin.y + magnitude*sin(theta)};
}

void PolarCoordinate::setMagnitude(float mag) { magnitude = mag; }
void PolarCoordinate::setTheta(float the) { theta = the; }
