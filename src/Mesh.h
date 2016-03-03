#ifndef MESH_H
#define MESH_H

#include <Eigen/Geometry>
#include "Shape.h"
#include "MatrixStack.h"
#include "Program.h"

class Mesh {
public:
   Mesh(std::string objFile, Eigen::Vector3f color, const std::shared_ptr<Program> prog);
   ~Mesh();
   
   /** Assumes the shape's program is currently bound. */
   void draw();
   void scale(const Eigen::Vector3f& scaleVals);
   void translate(const Eigen::Vector3f& trans);
   void rotate(float angle, const Eigen::Vector3f& axis);
   void rotateAboutOrigin(float angle, const Eigen::Vector3f& axis);
   void setMaterial(int i);

private:
   std::shared_ptr<Shape> _shape;
   std::shared_ptr<MatrixStack> _M;
   std::shared_ptr<Program> _prog;
   Eigen::Vector3f _color;
};

#endif