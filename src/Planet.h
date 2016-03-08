#ifndef PLANET_H
#define PLANET_H

#include <Eigen/Dense>
#include <Eigen/Geometry>

class Planet {
public:
   Planet(float radius, float initialAngle, float rotationTime, float orbitWidth,
      float orbitHeight, float orbitTime, const Eigen::Vector3f& location);
   ~Planet();

   Eigen::Vector3f getScale();
   Eigen::Vector3f getLocation(double curTime);
   float getRotation(double curTime);

private:
   float _radius, _initialAngle, _rotationTime, _orbitWidth, _orbitHeight, _orbitTime;
   Eigen::Vector3f _location;
};

class Star : Planet {
public:
   Star(float radius, float initialAngle, float rotationTime, float orbitWidth,
      float orbitHeight, float orbitTime, Eigen::Vector3f& location, Eigen::Vector3f& lightEmission)
   : Planet(radius, initialAngle, rotationTime, orbitWidth, orbitHeight, orbitTime, location) {
      _lightEmission = lightEmission;
   }
   ~Star();

private:
   Eigen::Vector3f _lightEmission;
};

#endif