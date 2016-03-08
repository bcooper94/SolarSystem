#include <math.h>

#include "Planet.h"

using namespace std;
using namespace Eigen;

// private:
//    float _radius;
   // Eigen::Vector3f _location;

Planet::Planet(float radius, float initialAngle, float rotationTime, float orbitWidth,
      float orbitHeight, float orbitTime, const Eigen::Vector3f& location) {
   _radius = radius;
   _initialAngle = initialAngle * M_PI / 180;
   _rotationTime = rotationTime;
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

float Planet::getInitialAngle() {
   return _initialAngle;
}

float Planet::getOrbitTime() {
   return _orbitTime;
}

float Planet::getOrbitWidth() {
   return _orbitWidth;
}

float Planet::getOrbitHeight() {
   return _orbitHeight;
}

Vector3f Planet::getLocation(double curTime) {
   return Vector3f(_location.x() + _orbitWidth * cos(_initialAngle + 2 * M_PI * curTime / _orbitTime),
      _location.y(),
      _location.z() + _orbitHeight * sin(_initialAngle + 2 * M_PI * curTime / _orbitTime));
}

float Planet::getRotation(double curTime) {
   return 2 * M_PI * curTime / _rotationTime;
}