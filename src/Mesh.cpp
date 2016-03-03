#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Mesh.h"

#define RESOURCE_DIR "resources/"

using namespace std;
using namespace Eigen;

Mesh::Mesh(string objFile, Vector3f color, const shared_ptr<Program> prog) {
   _shape = make_shared<Shape>();
   _shape->loadMesh(RESOURCE_DIR + objFile);
   _shape->resize();
   _shape->init();
   _prog = prog;
   _M = make_shared<MatrixStack>();
   _M->loadIdentity();
   cout << "Prog: " << prog << endl;
   _color = color;
}

Mesh::~Mesh() {

}

void Mesh::draw() {
   glUniform3f(_prog->getUniform("meshColor"), _color.x(), _color.y(), _color.z());
   glUniformMatrix4fv(_prog->getUniform("M"), 1, GL_FALSE, _M->topMatrix().data());
   _shape->draw(_prog);
}

void Mesh::scale(const Eigen::Vector3f& scaleVals) {
   _M->scale(scaleVals);
}

void Mesh::translate(const Eigen::Vector3f& trans) {
   _M->translate(trans);
}

void Mesh::rotate(float angle, const Eigen::Vector3f& axis) {
   _M->rotate(angle, axis);
}

void Mesh::rotateAboutOrigin(float angle, const Eigen::Vector3f& axis) {
}

void Mesh::setMaterial(int i) {
   int mat = i % 4;

  switch (mat) {
    case 0: //shiny blue plastic
       glUniform3f(_prog->getUniform("matAmbient"), 0.02, 0.04, 0.2);
       glUniform3f(_prog->getUniform("matDiffuse"), 0.0, 0.16, 0.9);
       glUniform3f(_prog->getUniform("matSpecular"), 0.14, 0.2, 0.8);
       glUniform1f(_prog->getUniform("specularPower"), 120.0);
      //set specular to: (0.14, 0.2, 0.8);
      //set shine to: (120.0);
        break;
    case 1: // flat grey
       glUniform3f(_prog->getUniform("matAmbient"), 0.13, 0.13, 0.14);
       glUniform3f(_prog->getUniform("matDiffuse"), 0.3, 0.3, 0.4);
       glUniform3f(_prog->getUniform("matSpecular"), 0.3, 0.3, 0.4);
       glUniform1f(_prog->getUniform("specularPower"), 4.0);
      //set specular to: (0.3, 0.3, 0.4);
      //set shine to: (4.0);
      break;
    case 2: //brass
       glUniform3f(_prog->getUniform("matAmbient"), 0.3294, 0.2235, 0.02745);
       glUniform3f(_prog->getUniform("matDiffuse"), 0.7804, 0.5686, 0.11373);
       glUniform3f(_prog->getUniform("matSpecular"), 0.9922, 0.941176, 0.80784);
       glUniform1f(_prog->getUniform("specularPower"), 27.9);
      //set specualr to: (0.9922, 0.941176, 0.80784);
      //set shine to: (27.9);
        break;
    case 3: //copper
       glUniform3f(_prog->getUniform("matAmbient"), 0.1913, 0.0735, 0.0225);
       glUniform3f(_prog->getUniform("matDiffuse"), 0.7038, 0.27048, 0.0828);
       glUniform3f(_prog->getUniform("matSpecular"), 0.257, 0.1376, 0.08601);
       glUniform1f(_prog->getUniform("specularPower"), 12.8);
      //set specualr to: (0.257, 0.1376, 0.08601);
      //set shine to: (12.8);
        break;
   }
}