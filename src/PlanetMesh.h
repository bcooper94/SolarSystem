#ifndef PLANET_MESH_H
#define PLANET_MESH_H

#include <Eigen/Geometry>
#include <iostream>

#include "Planet.h"
#include "Texture.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

class PlanetMesh {
public:
   PlanetMesh(const std::shared_ptr<Planet>& planet, const std::string& textureFile, int textureUnit);
   ~PlanetMesh();

   void init();
   void setMaterial(const Eigen::Vector3f& ambient, const Eigen::Vector3f& diffuse, const Eigen::Vector3f& specular, float shine);
   void draw(double curTime, std::shared_ptr<MatrixStack> view, std::shared_ptr<MatrixStack>& P);

private:
   std::shared_ptr<Planet> _planet;
   Texture _texture;
   std::shared_ptr<Program> _program;
   std::shared_ptr<Shape> _shape;
   Eigen::Vector3f _ambient, _diffuse, _specular;
   float _shine;
};

#endif