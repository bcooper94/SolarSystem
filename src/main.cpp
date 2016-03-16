/* Lab 5 base code - transforms using local matrix functions 
   to be written by students - 
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <math.h>
#include "main.h"
#include "Texture.h"
#include "Planet.h"
#include "PlanetMesh.h"

using namespace std;
using namespace Eigen;

#define ASTEROID_COUNT 5
#define OBJECT_DISTANCE 40

GLFWwindow *window; // Main application window
static string RESOURCE_DIR = "resources/"; // Where the resources are loaded from
static string IMG_DIR = "resources/img/";

static int textureCount = 0;

shared_ptr<Program> prog, backgroundProg, bloomProg;

Texture backgroundTexture;

vector<PlanetMesh> planetMeshes;
vector<PlanetMesh> asteroidMeshes;

int g_width, g_height;
float objectRotation, lightXTranslate, cameraBeta, cameraAlpha;
int gMat = 0;
bool mouseDown = false;
double xPosition, yPosition;
Vector3f eye, upDirection, lookAtPoint;
bool mouseActive = true;

int g_GiboLen;

double curTime;

//global data for background plane
GLuint BackgroundBuffObj, BackgroundNorBuffObj, BackgroundTexBuffObj, BackIndexBuffObj;

GLuint framebuffer = 0, renderBuffer;
map<bool, GLuint> pingpongFBO;

GLuint renderedTexture,
   depthTexture,
   // The fullscreen quad's FBO
   quad_VertexArrayID,
   quad_vertexbuffer,
   quad_elementBuffer;

GLuint brightTextures[2];

GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

shared_ptr<Program> quadProg;

GLubyte *data;

Texture test;

static float degreesToRad(float degrees) {
   return 3.1415 * degrees / 180;
}

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static float clamp(float value, float minVal, float maxVal) {
   return min(max(value, minVal), maxVal);
}

static void checkGLError(int lineNum) {
   GLenum error = glGetError();

   switch(error) {
      case GL_INVALID_ENUM:
         cout << "GLError: invalid enum on line" << lineNum << endl;
         break;
      case GL_INVALID_VALUE:
         cout << "GLError: invalid value on line " << lineNum << endl;
         break;
      case GL_INVALID_OPERATION:
         cout << "GLError: invalid operation on line " << lineNum << endl;
         break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
         cout << "GLError: invalid framebuffer operation on line " << lineNum << endl;
         break;
      case GL_OUT_OF_MEMORY:
         cout << "GLError: out of memory on line " << lineNum << endl;
         break;
      case GL_STACK_UNDERFLOW:
         cout << "GLError: stack underflow on line " << lineNum << endl;
         break;
      case GL_STACK_OVERFLOW:
         cout << "GLError: stack overflow on line " << lineNum << endl;
         break;
   }
}

void SetMaterial(int mat) {
  switch (mat) {
    case 0: //shiny blue plastic
      glUniform3f(prog->getUniform("matAmbient"), 0.02, 0.04, 0.2);
      glUniform3f(prog->getUniform("matDiffuse"), 0.0, 0.16, 0.9);
      glUniform3f(prog->getUniform("matSpecular"), 0.14, 0.2, 0.8);
      glUniform1f(prog->getUniform("specularPower"), 120.0);
      //set specular to: (0.14, 0.2, 0.8);
      //set shine to: (120.0);
      break;
    case 1: // flat grey
      glUniform3f(prog->getUniform("matAmbient"), 0.13, 0.13, 0.14);
      glUniform3f(prog->getUniform("matDiffuse"), 0.3, 0.3, 0.4);
      glUniform3f(prog->getUniform("matSpecular"), 0.3, 0.3, 0.4);
      glUniform1f(prog->getUniform("specularPower"), 4.0);
      //set specular to: (0.3, 0.3, 0.4);
      //set shine to: (4.0);
      break;
    case 2: //brass
      glUniform3f(prog->getUniform("matAmbient"), 0.3294, 0.2235, 0.02745);
      glUniform3f(prog->getUniform("matDiffuse"), 0.7804, 0.5686, 0.11373);
      glUniform3f(prog->getUniform("matSpecular"), 0.9922, 0.941176, 0.80784);
      glUniform1f(prog->getUniform("specularPower"), 27.9);
      //set specualr to: (0.9922, 0.941176, 0.80784);
      //set shine to: (27.9);
      break;
    case 3: //copper
      glUniform3f(prog->getUniform("matAmbient"), 0.1913, 0.0735, 0.0225);
      glUniform3f(prog->getUniform("matDiffuse"), 0.7038, 0.27048, 0.0828);
      glUniform3f(prog->getUniform("matSpecular"), 0.257, 0.1376, 0.08601);
      glUniform1f(prog->getUniform("specularPower"), 12.8);
      //set specualr to: (0.257, 0.1376, 0.08601);
      //set shine to: (12.8);
      break;
    case 4: // grass
      glUniform3f(prog->getUniform("matAmbient"), 0.01, 0.05, 0.02);
      glUniform3f(prog->getUniform("matDiffuse"), 0.07, 0.3, 0.1);
      glUniform3f(prog->getUniform("matSpecular"), 0.1, 0.25, 0.15);
      glUniform1f(prog->getUniform("specularPower"), 9.8);
      break;
    case 5: // silver
      glUniform3f(prog->getUniform("matAmbient"), 0.19225, 0.19225, 0.19225);
      glUniform3f(prog->getUniform("matDiffuse"), 0.50754, 0.50754, 0.50754);
      glUniform3f(prog->getUniform("matSpecular"), 0.508273, 0.508273, 0.508273);
      glUniform1f(prog->getUniform("specularPower"), 51.2);
      break;
    case 6: // ruby
      glUniform3f(prog->getUniform("matAmbient"), 0.1745, 0.01175, 0.01175);
      glUniform3f(prog->getUniform("matDiffuse"), 0.61424, 0.04136, 0.04136);
      glUniform3f(prog->getUniform("matSpecular"), 0.727811, 0.626959, 0.626959);
      glUniform1f(prog->getUniform("specularPower"), 76.8);
      break;
    case 7: // gold
      glUniform3f(prog->getUniform("matAmbient"), 0.24725, 0.1995, 0.0745);
      glUniform3f(prog->getUniform("matDiffuse"), 0.75164, 0.60648, 0.22648);
      glUniform3f(prog->getUniform("matSpecular"), 0.628281, 0.555802, 0.366065);
      glUniform1f(prog->getUniform("specularPower"), 51.2);
      break;
    case 8: // sun
      glUniform3f(prog->getUniform("matAmbient"), 1, 0.7, 0);
      glUniform3f(prog->getUniform("matDiffuse"), 0, 0, 0);
      glUniform3f(prog->getUniform("matSpecular"), 0, 0, 0);
      glUniform1f(prog->getUniform("specularPower"), 51.2);
      break;
    case 9: // planets
      glUniform3f(prog->getUniform("matAmbient"), 1, 0.7, 0);
      glUniform3f(prog->getUniform("matDiffuse"), 0, 0, 0);
      glUniform3f(prog->getUniform("matSpecular"), 0, 0, 0);
      glUniform1f(prog->getUniform("specularPower"), 51.2);
      break;
   }
}

static void switchMouseActive(GLFWwindow *window) {
   if (mouseActive) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }
   else {
      double xPos, yPos;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwGetCursorPos(window, &xPos, &yPos);
      xPosition = xPos;
      yPosition = yPos;
   }

   mouseActive = !mouseActive;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
   if (glfwGetKey(window, GLFW_KEY_E)) {
      lightXTranslate += 0.1;
   }
   if (glfwGetKey(window, GLFW_KEY_Q)) {
      lightXTranslate -= 0.1;
   }
   if (glfwGetKey(window, GLFW_KEY_M) && action == GLFW_PRESS) {
      gMat = (gMat + 1) % 4;
   }
   if (glfwGetKey(window, GLFW_KEY_W)) {
      eye += (lookAtPoint - eye) * 0.3;
   }
   if (glfwGetKey(window, GLFW_KEY_A)) {
      Vector3f w = eye - lookAtPoint;
      eye -= upDirection.cross(w) * 0.3;
   }
   if (glfwGetKey(window, GLFW_KEY_S)) {
      eye -= (lookAtPoint - eye) * 0.3;
   }
   if (glfwGetKey(window, GLFW_KEY_D)) {
      Vector3f w = eye - lookAtPoint;
      eye += upDirection.cross(w) * 0.3;
   }
   if (glfwGetKey(window, GLFW_KEY_SPACE)) {
      eye += upDirection * 0.3;
   }
   if (glfwGetKey(window, GLFW_KEY_X)) {
      eye -= upDirection * 0.3;
   }
   if (glfwGetKey(window, GLFW_KEY_TAB)) {
      switchMouseActive(window);
   }
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods) {
   cout << "Eye loc: (" << eye.x() << ", " << eye.y() << ", " << eye.z() << ")\n";
   cout << "LookAt: (" << lookAtPoint.x() << ", " << lookAtPoint.y() << ", " << lookAtPoint.z() << ")\n";
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
   if (mouseActive) {
      double xChange = xpos - xPosition, yChange = ypos - yPosition;
      cameraAlpha = clamp(cameraAlpha - yChange / g_height, degreesToRad(-85), degreesToRad(85));
      cameraBeta = cameraBeta + xChange / g_width;

      xPosition = xpos;
      yPosition = ypos;
   }
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}

static void initGeom() {
   // A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
   float BackgroundPos[] = {
      // upper left
      -1.0, -1.0, -1.0,
      -1.0, 1.0, -1.0,
      1.0, 1.0, -1.0,

      // lower right
      -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, -1.0f
   };

   float BackgroundNorm[] = {
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,

      0, 1, 0,
      0, 1, 0,
      0, 1, 0
   };


  static GLfloat BackgroundTex[] = {
      0, 0, // back
      0, 5,
      5, 5,
      5, 0,
      0, 0,
      0, 5 };

   unsigned short idx[] = {0, 1, 2, 0, 2, 3};


   GLuint VertexArrayID;
   //generate the VAO
   glGenVertexArrays(1, &VertexArrayID);
   glBindVertexArray(VertexArrayID);

   g_GiboLen = 6;
   glGenBuffers(1, &BackgroundBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, BackgroundBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(BackgroundPos), BackgroundPos, GL_STATIC_DRAW);

   glGenBuffers(1, &BackgroundNorBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, BackgroundNorBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(BackgroundNorm), BackgroundNorm, GL_STATIC_DRAW);

   glGenBuffers(1, &BackgroundTexBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, BackgroundTexBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(BackgroundTex), BackgroundTex, GL_STATIC_DRAW);

   glGenBuffers(1, &BackIndexBuffObj);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BackIndexBuffObj);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
}

static shared_ptr<Shape> initShape(string objFilename) {
   // Initialize mesh.
   shared_ptr<Shape> shape = make_shared<Shape>();
   shape->loadMesh(RESOURCE_DIR + objFilename);
   shape->resize();
   shape->init();

   return shape;
}

static void init()
{
   GLSL::checkVersion();

   objectRotation = 0;
   lightXTranslate = -2;
   eye = Vector3f(0, 0, 8.5);
   upDirection = Vector3f(0, 1, 0);
   xPosition = 0;
   yPosition = 0;
   // Set background color.
   glClearColor(1.0f, 0.0f, 0.0f, 0.5f);
   // Enable z-buffer test.
   glEnable(GL_DEPTH_TEST);
   
   quadProg = make_shared<Program>();
   quadProg->setVerbose(true);
   quadProg->setShaderNames(RESOURCE_DIR + "render_vert.glsl", RESOURCE_DIR + "render_frag.glsl");
   quadProg->init();
   checkGLError(368);
   quadProg->addUniform("renderedTexture");
   quadProg->addUniform("brightTexture");
   quadProg->addUniform("windowWidth");
   quadProg->addUniform("windowHeight");
   quadProg->addAttribute("vertPos");

   setupRenderTexture();
   setupQuadBuffer();
   setupPingPongFBO();

   bloomProg = make_shared<Program>();
   bloomProg->setVerbose(true);
   bloomProg->setShaderNames(RESOURCE_DIR + "bloom_vert.glsl", RESOURCE_DIR + "bloom_frag.glsl");
   bloomProg->init();
   checkGLError(368);
   bloomProg->addUniform("renderedTexture");
   bloomProg->addUniform("windowWidth");
   bloomProg->addUniform("windowHeight");
   bloomProg->addUniform("horizontal");
   bloomProg->addAttribute("vertPos");

   // Initialize the GLSL program.
   prog = make_shared<Program>();
   prog->setVerbose(false);
   prog->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
   prog->init();
   prog->addUniform("P");
   prog->addUniform("V");
   prog->addUniform("M");
   prog->addUniform("curTime");
   prog->addUniform("curTime");
   prog->addUniform("orbitDimensions");
   prog->addUniform("orbitTime");
   prog->addUniform("initialAngle");
   prog->addUniform("lightPos");
   // prog->addUniform("lightDirection");
   prog->addUniform("lightColor");
   prog->addUniform("matAmbient");
   prog->addUniform("matDiffuse");
   prog->addUniform("matSpecular");
   prog->addUniform("specularPower");
   prog->addUniform("attenuation");
   prog->addAttribute("vertPos");
   prog->addAttribute("vertNor");
   prog->addAttribute("vertTex");

   backgroundTexture = Texture();

   backgroundTexture.setFilename(IMG_DIR + "starry_sky.bmp");
   backgroundTexture.setUnit(0);
   backgroundTexture.setName("StarTexture");
   backgroundTexture.init();

   backgroundProg = make_shared<Program>();
   backgroundProg->setVerbose(false);
   backgroundProg->setShaderNames(RESOURCE_DIR + "background_vert.glsl", RESOURCE_DIR + "background_frag.glsl");
   backgroundProg->init();
   backgroundProg->addUniform("P");
   backgroundProg->addUniform("M");
   backgroundProg->addUniform("StarTexture");
   backgroundProg->addUniform("windowSize");
   backgroundProg->addAttribute("vertPos");
   backgroundProg->addAttribute("vertNor");
   backgroundProg->addAttribute("vertTex");
   backgroundProg->addTexture(&backgroundTexture);
}

static void initPlanets() {
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(9.0, 35, 1.0, 0, 0, 1,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "sun.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(0.65, 41, 2.0, 11.0, 11.0, 14.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "mercury.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(0.8, 23, 2.0, 13.0, 13.0, 40.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "venus.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(1.1, 18, 2.0, 16.0, 16.0, 55.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "earth.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(1.0, 30, 2.0, 19.0, 19.0, 90.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "mars.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(5.0, 79, 2.0, 45.0, 45.0, 250.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)),  "jupiter.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(4.5, 5, 2.0, 58.0, 58.0, 350.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "saturn.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(2.4, 110, 2.0, 71.0, 71.0, 500.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "uranus.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(2.5, 275, 2.0, 78.0, 78.0, 600.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "neptune.bmp", "sphere.obj", textureCount));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(0.5, 323, 2.0, 94.0, 82.0, 700.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "pluto.bmp", "sphere.obj", textureCount));

   planetMeshes[0].init();
   // planetMeshes[0].setMaterial(Vector3f(1, 0.7, 0), Vector3f(0, 0, 0), Vector3f(0, 0, 0), 51.2);
   planetMeshes[0].setMaterial(Vector3f(15, 12, 0), Vector3f(0, 0, 0), Vector3f(0, 0, 0), 51.2);

   for (size_t index = 1; index < planetMeshes.size(); index++) {
      planetMeshes[index].init();
      planetMeshes[index].setMaterial(Vector3f(0.19, 0.19, 0.2),
         Vector3f(0.3, 0.3, 0.4),
         Vector3f(0.5, 0.5, 0.6), 9.0);
   }
}

static void initAsteroids() {
   Texture asteroidText;
   float angle, x, y, height, distance, radius,
   distFromSun = 30.0, distRange = 3.25,
      heightRange = 2.5, radiusMid = 0.18, radiusRange = 0.1;

   shared_ptr<Shape> asteroidModel = make_shared<Shape>();
   asteroidModel->loadMesh(RESOURCE_DIR + "asteroid.obj");
   asteroidModel->resize();
   asteroidModel->init();

   asteroidText.setFilename(IMG_DIR + "mercury.bmp");
   asteroidText.setUnit(textureCount);
   asteroidText.setName("Texture");
   asteroidText.init();

   for (size_t index = 0; index < ASTEROID_COUNT; index++) {
      angle = ((float)rand() / RAND_MAX) * 360;
      distance = distFromSun - distRange + ((float)rand() / RAND_MAX) * 2 * distRange;
      x = distance * cos(angle);
      y = distance * sin(angle);
      height = ((float)rand() / RAND_MAX) * heightRange;
      radius = radiusMid + ((float)rand() / RAND_MAX) * radiusRange;
      // cout << "Creating asteroid: " << index << "at angle: " << angle << endl;

      asteroidMeshes.push_back(PlanetMesh(make_shared<Planet>(radius, angle, 10.0, distance, distance, 50.0,
         Vector3f(OBJECT_DISTANCE, height, 0)), asteroidText, asteroidModel));
   }

   for (size_t index = 0; index < ASTEROID_COUNT; index++) {
      asteroidMeshes[index].setMaterial(Vector3f(0.13, 0.13, 0.14),
         Vector3f(0.3, 0.3, 0.4),
         Vector3f(0.25, 0.25, 0.30), 1.0);
      asteroidMeshes[index].init();
   }
}

static shared_ptr<MatrixStack> getView() {
   float x = cos(cameraAlpha) * cos(cameraBeta),
         y = sin(cameraAlpha), z = cos(cameraAlpha) * cos(3.1415 / 2 - cameraBeta);

   lookAtPoint = Vector3f(x, y, z) + eye;
   auto view = make_shared<MatrixStack>();
   view->pushMatrix();
   view->lookAt(eye, lookAtPoint, upDirection);

   return view;
}

static void printMidPixels() {
   cout << "Printing pixels:\n";
   for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 10; j++) {
         printf("%d ", data[g_width / 2 - 5 + i + g_width * (g_height / 2 - 5 + j)]);
      }
   }
   cout << endl;
}

static void draw(shared_ptr<Program>& program,
      shared_ptr<Shape>& shape, shared_ptr<MatrixStack>& P,
      shared_ptr<MatrixStack>& M) {
   
   shared_ptr<MatrixStack> view = getView();

   glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
   glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
   glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, view->topMatrix().data());
   glUniform3f(program->getUniform("lightPos"), 0, 0, -6);
   // glUniform3f(program->getUniform("lightDirection"), -0.2, 0.2, 1);
   glUniform3f(program->getUniform("lightColor"), 1, 1, 1);
   glUniform3f(program->getUniform("attenuation"), 1, 0, 0);
   shape->draw(program);

   view->popMatrix();
}

static void drawBackground(shared_ptr<MatrixStack>& P) {
   shared_ptr<MatrixStack> M = make_shared<MatrixStack>();
   M->loadIdentity();

   backgroundProg->bind();
   glUniformMatrix4fv(backgroundProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
   glUniformMatrix4fv(backgroundProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
   glUniform2f(backgroundProg->getUniform("windowSize"), g_width, g_height);

   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, BackgroundBuffObj);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, BackgroundNorBuffObj);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, BackgroundTexBuffObj);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   // draw!
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BackIndexBuffObj);
   glDrawArrays(GL_TRIANGLES, 0, 6);
   // glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
   
   backgroundProg->unbind();
}

static void drawPlanets(shared_ptr<MatrixStack>& P, shared_ptr<MatrixStack> view) {
   int planetCount = planetMeshes.size();

   for (size_t index = 0; index < planetCount; index++) {
      planetMeshes[index].draw(curTime, view, P);
   }
}

static void drawAsteroids(shared_ptr<MatrixStack>& P, shared_ptr<MatrixStack> view) {
   for (size_t index = 0; index < ASTEROID_COUNT; index++) {
      asteroidMeshes[index].draw(curTime, view, P);
   }
}

static void setupRenderTexture() {
   data = (GLubyte *)malloc(16 * g_width * g_height);

   renderedTexture = createTexture(g_width, g_height, false);
   brightTextures[0] = createTexture(g_width, g_height, false);
   brightTextures[1] = createTexture(g_width, g_height, false);
   // depthTexture = createTexture(g_width, g_height, true);

   glGenFramebuffers(1, &framebuffer);
   checkGLError(567);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
   checkGLError(569);

   // glGenRenderbuffers(1, &renderBuffer);
   // checkGLError(572);
   // glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
   // checkGLError(574);
   // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, g_width, g_height);
   // checkGLError(576);
   // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
   // checkGLError(578);

   glDrawBuffers(sizeof(drawBuffers), drawBuffers);
   checkGLError(581);

   // set renderedTexture as color attachment 0
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
   checkGLError(585);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightTextures[0], 0);
   checkGLError(584);

   assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   checkGLError(590);
   glBindTexture(GL_TEXTURE_2D, 0);
   checkGLError(592);
}

static void setupPingPongFBO() {
   bool buf = true;
   GLenum draw[1] = {GL_COLOR_ATTACHMENT0};

   for (int i = 0; i < 2; i++) {
      glGenFramebuffers(1, &pingpongFBO[buf]);
      glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[buf]);

      glDrawBuffers(sizeof(draw), draw);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brightTextures[i], 0);
      assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      
      buf = !buf;
   }
}

static unsigned int createTexture(int width, int height, bool isDepth) {
   unsigned int textureId;

   glGenTextures(1, &textureId);
   checkGLError(595);
   glBindTexture(GL_TEXTURE_2D, textureId);
   checkGLError(597);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   checkGLError(601);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   checkGLError(603);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   checkGLError(605);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   checkGLError(607);
   glTexImage2D(GL_TEXTURE_2D, 0, isDepth ? GL_DEPTH_COMPONENT : GL_RGB,
      width, height, 0, isDepth ? GL_DEPTH_COMPONENT : GL_RGB, GL_FLOAT, NULL);
   checkGLError(611);

   assert(glGetError() == 0);

   return textureId;
}

static void setupQuadBuffer() {
   static const GLfloat g_quad_vertex_buffer_data[] = {
      -1.0f, 1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      1.0f, 1.0f, 0.0f
   };
   unsigned int idx[] = { 0, 1, 2, 0, 2, 3 };

   glGenBuffers(1, &quad_vertexbuffer);
   checkGLError(593);
   glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
   checkGLError(595);
   glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
   checkGLError(597);

   glGenBuffers(1, &quad_elementBuffer);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_elementBuffer);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


   glGenVertexArrays(1, &quad_VertexArrayID);
   checkGLError(579);
   glBindVertexArray(quad_VertexArrayID);
   checkGLError(581);
}

static void drawRenderedTexture(int width, int height) {
   glDepthMask(GL_FALSE);
   checkGLError(657);
   glDisable(GL_DEPTH_TEST);
   checkGLError(659);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   checkGLError(662);

   glViewport(0, 0, width, height);
   checkGLError(666);
   glClear(GL_COLOR_BUFFER_BIT);
   checkGLError(668);
   glClearColor(0, 0, 1.0, 0.5);

   glBindVertexArray(quad_VertexArrayID);
   checkGLError(666);

   quadProg->bind();
   checkGLError(668);
   checkGLError(670);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, renderedTexture);
   glUniform1i(quadProg->getUniform("renderedTexture"), 0);

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, brightTextures[0]);
   glUniform1i(quadProg->getUniform("brightTexture"), 1);

   drawQuad(quadProg, width, height);
   // test.bind();
   // checkGLError(672);

   // // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, data);
   // // printMidPixels();

   // glUniform1i(quadProg->getUniform("renderedTexture"), 0);
   // glUniform1i(quadProg->getUniform("windowWidth"), width);
   // glUniform1i(quadProg->getUniform("windowHeight"), height);

   // glEnableVertexAttribArray(0);
   // glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
   // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // // draw!
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_elementBuffer);
   // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   
   // // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   // checkGLError(676);
   // // glDrawArrays(GL_TRIANGLES, 0, 6);

   // glDisableVertexAttribArray(0);
   // // glDisableVertexAttribArray(1);
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   // glBindTexture(GL_TEXTURE_2D, 0);
   // checkGLError(699);

   quadProg->unbind();
   checkGLError(602);
}

static void blurTexture(int width, int height) {
   GLboolean isHorizontal = true, first_iteration = true;
   GLuint amount = 20;

   glDepthMask(GL_FALSE);
   checkGLError(657);
   glDisable(GL_DEPTH_TEST);
   checkGLError(659);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   checkGLError(662);

   glViewport(0, 0, width, height);
   checkGLError(666);
   glClear(GL_COLOR_BUFFER_BIT);
   checkGLError(668);
   glClearColor(0, 0, 1.0, 0.5);

   bloomProg->bind();

   for (GLuint i = 0; i < amount; i++)
   {
      glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[isHorizontal]); 
      glBindTexture(GL_TEXTURE_2D, first_iteration ? brightTextures[0] : brightTextures[1]);
      isHorizontal = !isHorizontal;
      glUniform1i(bloomProg->getUniform("horizontal"), isHorizontal ? 1 : 0);
      drawQuad(bloomProg, width, height);
      
      if (first_iteration) {
         first_iteration = false;
      }
   }


   // drawQuad(bloomProg, width, height);



   // glBindVertexArray(quad_VertexArrayID);
   // checkGLError(666);

   // // bloomProg->bind();
   // checkGLError(668);
   // // glActiveTexture(GL_TEXTURE0);
   // checkGLError(670);
   // glBindTexture(GL_TEXTURE_2D, brightTextures[0]);
   // // test.bind();
   // checkGLError(672);

   // // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, data);
   // // printMidPixels();

   // glUniform1i(bloomProg->getUniform("renderedTexture"), 0);
   // glUniform1i(bloomProg->getUniform("windowWidth"), width);
   // glUniform1i(bloomProg->getUniform("windowHeight"), height);

   // glEnableVertexAttribArray(0);
   // glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
   // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // // draw!
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_elementBuffer);
   // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   
   // // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   // checkGLError(676);
   // // glDrawArrays(GL_TRIANGLES, 0, 6);

   // glDisableVertexAttribArray(0);
   // // glDisableVertexAttribArray(1);
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   // glBindTexture(GL_TEXTURE_2D, 0);
   checkGLError(699);

   bloomProg->unbind();
   checkGLError(602);
}

static void drawQuad(shared_ptr<Program>& prog, int width, int height) {
   glBindVertexArray(quad_VertexArrayID);
   checkGLError(666);

   // bloomProg->bind();
   checkGLError(668);
   // glActiveTexture(GL_TEXTURE0);
   checkGLError(670);
   glBindTexture(GL_TEXTURE_2D, brightTextures[0]);
   // test.bind();
   checkGLError(672);

   // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, data);
   // printMidPixels();

   glUniform1i(prog->getUniform("renderedTexture"), 0);
   glUniform1i(prog->getUniform("windowWidth"), width);
   glUniform1i(prog->getUniform("windowHeight"), height);

   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // draw!
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_elementBuffer);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   
   // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   checkGLError(676);
   // glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   // glDisableVertexAttribArray(1);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   glBindTexture(GL_TEXTURE_2D, 0);
}

static void render()
{
	int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glViewport(0, 0, g_width, g_height);
   checkGLError(706);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
   checkGLError(708);

   glBindTexture(GL_TEXTURE_2D, 0);

   curTime = glfwGetTime();


   glDepthMask(GL_TRUE);

   float aspect = width/(float)height;
   auto P = make_shared<MatrixStack>();
   auto M = make_shared<MatrixStack>();
   shared_ptr<MatrixStack> view = getView();

   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 500.0f);

   glDisable(GL_DEPTH_TEST);
   drawBackground(P);

   glEnable(GL_DEPTH_TEST);

   drawPlanets(P, view);
   // glReadPixels(width / 2 - 5, height / 2 - 5, width / 2 + 5, height / 2 + 5, GL_RGB, GL_FLOAT, data);
   // printMidPixels();
   // drawAsteroids(P, view);

   P->popMatrix();
   blurTexture(width, height);
   drawRenderedTexture(width, height);
}

int main(int argc, char **argv)
{
	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(1024, 768, "Brandon Cooper", NULL, NULL);
  g_width = 1024;
  g_height = 768;

	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
   glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);

	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
   //set the mouse call back
   glfwSetMouseButtonCallback(window, mouse_callback);
   glfwSetCursorPosCallback(window, cursor_position_callback);
   //set the window resize call back
   glfwSetFramebufferSizeCallback(window, resize_callback);

   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize scene. Note geometry initialized in init now
	init();
   initGeom();
   initPlanets();
   // initAsteroids();

   // if (!setupRenderTexture()) {
   //    cout << "Framebuffer error\n";
   //    return -1;
   // }

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
