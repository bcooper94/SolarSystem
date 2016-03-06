#include <math.h>

#include "Planet.h"

using namespace std;
using namespace Eigen;

// private:
//    float _radius;
   // Eigen::Vector3f _location;

Planet::Planet(float radius, float rotationalV, float orbitWidth,
      float orbitHeight, float orbitTime, const Eigen::Vector3f& location) {
   _radius = radius;
   _rotationalV = rotationalV;
   _orbitWidth = orbitWidth;
   _orbitHeight = orbitHeight;
   _orbitTime = orbitTime;
   _location = location;
}

Planet::~Planet() {
}

Vector3f Planet::getScale() {
   return Vector3f(_radius, _radius, _radius);
}

Vector3f Planet::getLocation(double curTime) {
   return Vector3f(_location.x() + _orbitWidth * cos(2 * M_PI * curTime / _orbitTime),
      _location.y(),
      _location.z() + _orbitHeight * sin(2 * M_PI * curTime / _orbitTime));
}