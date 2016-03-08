#include "PlanetMesh.h"

using namespace std;
using namespace Eigen;

static string RESOURCE_DIR = "resources/";
static string IMG_DIR = "resources/img/";

PlanetMesh::PlanetMesh(const shared_ptr<Planet>& planet, const string& textureFile,
   const string& objFile, int textureUnit) {
   _planet = planet;
   _texture.setFilename(IMG_DIR + textureFile);
   _texture.setUnit(textureUnit);
   _texture.setName("Texture");
   _texture.init();

   _shape = make_shared<Shape>();
   _shape->loadMesh(RESOURCE_DIR + objFile);
   _shape->resize();
   _shape->init();
}

PlanetMesh::PlanetMesh(const shared_ptr<Planet>& planet, Texture& texture,
   shared_ptr<Shape> shape) {
   _planet = planet;
   _texture = texture;
   _shape = shape;
}

PlanetMesh::~PlanetMesh() {
}

void PlanetMesh::init() {
   _program = make_shared<Program>();
   _program->setVerbose(true);
   _program->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "texture_frag.glsl");
   _program->init();
   _program->addUniform("P");
   _program->addUniform("V");
   _program->addUniform("M");
   _program->addUniform("curTime");
   _program->addUniform("orbitDimensions");
   _program->addUniform("orbitTime");
   _program->addUniform("initialAngle");
   _program->addUniform("lightPos");
   _program->addUniform("lightColor");
   _program->addUniform("matAmbient");
   _program->addUniform("matDiffuse");
   _program->addUniform("matSpecular");
   _program->addUniform("specularPower");
   _program->addUniform("attenuation");
   _program->addAttribute("vertPos");
   _program->addAttribute("vertNor");
   _program->addAttribute("vertTex");
   _program->addUniform("Texture");
   _program->addTexture(&_texture);
}

void PlanetMesh::setMaterial(const Vector3f& ambient, const Vector3f& diffuse, const Vector3f& specular, float shine) {
   _ambient = ambient;
   _diffuse = diffuse;
   _specular = specular;
   _shine = shine;
}

void PlanetMesh::draw(double curTime, shared_ptr<MatrixStack> view, shared_ptr<MatrixStack>& P) {
   shared_ptr<MatrixStack> M = make_shared<MatrixStack>();

   _program->bind();

   M->pushMatrix();
   M->translate(_planet->getLocation(curTime));
   M->scale(_planet->getScale());
   M->rotate(_planet->getRotation(curTime), Vector3f(0, 1, 0));

   glUniformMatrix4fv(_program->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
   glUniformMatrix4fv(_program->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
   glUniformMatrix4fv(_program->getUniform("V"), 1, GL_FALSE, view->topMatrix().data());
   glUniform1f(_program->getUniform("curTime"), curTime);
   glUniform2f(_program->getUniform("orbitDimensions"), _planet->getOrbitWidth(), _planet->getOrbitHeight());
   glUniform1f(_program->getUniform("orbitTime"), _planet->getOrbitTime());
   glUniform1f(_program->getUniform("initialAngle"), _planet->getInitialAngle());
   glUniform3f(_program->getUniform("lightPos"), 0, 0, -6);
   // glUniform3f(_program->getUniform("lightDirection"), -0.2, 0.2, 1);
   glUniform3f(_program->getUniform("lightColor"), 1, 1, 1);
   glUniform3f(_program->getUniform("attenuation"), 1, 0, 0);
   glUniform3f(_program->getUniform("matAmbient"), _ambient.x(), _ambient.y(), _ambient.z());
   glUniform3f(_program->getUniform("matDiffuse"), _diffuse.x(), _diffuse.y(), _diffuse.z());
   glUniform3f(_program->getUniform("matSpecular"), _specular.x(), _specular.y(), _specular.z());
   glUniform1f(_program->getUniform("specularPower"), _shine);
   
   _shape->draw(_program);
   M->popMatrix();

   _program->unbind();
}

// private:
//    Planet _planet;
//    Texture _texture;
//    shared_ptr<Program> _program;
//    shared_ptr<Shape> _shape;
//    Vector3f _ambient, _diffuse, _specular;
//    float _shine;