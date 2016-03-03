#ifndef PLANET_H
#define PLANET_H

#include <Eigen/Dense>
#include <Eigen/Geometry>

class Planet {
public:
   Planet(float radius, float rotationalV, float orbitWidth,
      float orbitHeight, float orbitTime, const Eigen::Vector3f& location);
   ~Planet();

   Eigen::Vector3f getLocation(double curTime);

private:
   float _radius, _rotationalV, _orbitWidth, _orbitHeight, _orbitTime;
   Eigen::Vector3f _location;
};

class Star : Planet {
public:
   Star(float radius, float rotationalV, float orbitWidth, float orbitHeight,
      float orbitTime, Eigen::Vector3f& location, Eigen::Vector3f& lightEmission)
   : Planet(radius, rotationalV, orbitWidth, orbitHeight, orbitTime, location) {
      _lightEmission = lightEmission;
   }
   ~Star();

private:
   Eigen::Vector3f _lightEmission;
};

#endif