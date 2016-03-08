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

#define ASTEROID_COUNT 900
#define OBJECT_DISTANCE 40

GLFWwindow *window; // Main application window
static string RESOURCE_DIR = "resources/"; // Where the resources are loaded from
static string IMG_DIR = "resources/img/";

static int textureCount = 0;

shared_ptr<Program> prog;
shared_ptr<Program> backgroundProg;

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


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
 //   if (action == GLFW_PRESS) {
 //      glfwGetCursorPos(window, &xPosition, &yPosition);
 //      cout << "Pos X " << xPosition <<  " Pos Y " << yPosition << endl;
 //      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
 //      mouseDown = true;
	// }
 //   else if (action == GLFW_RELEASE) {
 //      mouseDown = false;
 //      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
 //   }
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
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   // Enable z-buffer test.
   glEnable(GL_DEPTH_TEST);

   // initMeshes();

   // Initialize the GLSL program.
   prog = make_shared<Program>();
   prog->setVerbose(true);
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
   backgroundProg->setVerbose(true);
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

static shared_ptr<MatrixStack> getView() {
   float x = cos(cameraAlpha) * cos(cameraBeta),
         y = sin(cameraAlpha), z = cos(cameraAlpha) * cos(3.1415 / 2 - cameraBeta);

   lookAtPoint = Vector3f(x, y, z) + eye;
   auto view = make_shared<MatrixStack>();
   view->pushMatrix();
   view->lookAt(eye, lookAtPoint, upDirection);

   return view;
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

static void initPlanets() {
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(9.0, 35, 1.0, 0, 0, 1,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "sun.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(0.65, 41, 2.0, 11.0, 11.0, 14.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "mercury.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(0.8, 23, 2.0, 13.0, 13.0, 40.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "venus.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(1.1, 18, 2.0, 16.0, 16.0, 55.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "earth.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(1.0, 63, 2.0, 19.0, 19.0, 90.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "mars.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(5.0, 79, 2.0, 40.0, 45.0, 250.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)),  "jupiter.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(4.5, 86, 2.0, 50.0, 50.0, 350.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "saturn.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(2.4, 110, 2.0, 63.0, 63.0, 500.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "uranus.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(2.5, 275, 2.0, 72.0, 72.0, 600.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "neptune.bmp", "alt_sphere.obj", textureCount++));
   planetMeshes.push_back(PlanetMesh(make_shared<Planet>(0.5, 323, 2.0, 88.0, 76.0, 700.0,
      Vector3f(OBJECT_DISTANCE, 0, 0)), "pluto.bmp", "alt_sphere.obj", textureCount++));

   planetMeshes[0].init();
   planetMeshes[0].setMaterial(Vector3f(1, 0.7, 0), Vector3f(0, 0, 0), Vector3f(0, 0, 0), 51.2);

   for (size_t index = 1; index < planetMeshes.size(); index++) {
      planetMeshes[index].init();
      planetMeshes[index].setMaterial(Vector3f(0.19, 0.19, 0.2),
         Vector3f(0.3, 0.3, 0.4),
         Vector3f(0.5, 0.5, 0.6), 9.0);
   }
}

static void initAsteroids() {
   Texture asteroidText;
   float angle, x, y, height, distance, distFromSun = 30.0, distRange = 3.25,
      heightRange = 2.5;

   shared_ptr<Shape> asteroidModel = make_shared<Shape>();
   asteroidModel->loadMesh(RESOURCE_DIR + "asteroid.obj");
   asteroidModel->resize();
   asteroidModel->init();

   asteroidText.setFilename(IMG_DIR + "mercury.bmp");
   asteroidText.setUnit(textureCount++);
   asteroidText.setName("Texture");
   asteroidText.init();

   for (size_t index = 0; index < ASTEROID_COUNT; index++) {
      angle = ((float)rand() / RAND_MAX) * 360;
      distance = distFromSun - distRange + ((float)rand() / RAND_MAX) * 2 * distRange;
      x = distance * cos(angle);
      y = distance * sin(angle);
      height = ((float)rand() / RAND_MAX) * heightRange;
      // cout << "Creating asteroid: " << index << "at angle: " << angle << endl;

      asteroidMeshes.push_back(PlanetMesh(make_shared<Planet>(0.3, angle, 10.0, distance, distance, 25.0,
         Vector3f(OBJECT_DISTANCE, height, 0)), asteroidText, asteroidModel));
   }

   for (size_t index = 0; index < ASTEROID_COUNT; index++) {
      asteroidMeshes[index].setMaterial(Vector3f(1, 0.7, 0), Vector3f(0, 0, 0), Vector3f(0, 0, 0), 51.2);
      asteroidMeshes[index].init();
   }
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

static void render()
{
	// Get current frame buffer size.
	int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   glViewport(0, 0, width, height);

   curTime = glfwGetTime();

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;

   // Create the matrix stacks - please leave these alone for now
   auto P = make_shared<MatrixStack>();
   auto M = make_shared<MatrixStack>();
   shared_ptr<MatrixStack> view = getView();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 500.0f);

   glDisable(GL_DEPTH_TEST);
   drawBackground(P);

   glEnable(GL_DEPTH_TEST);

   drawPlanets(P, view);
   drawAsteroids(P, view);

   // Pop matrix stacks.
   P->popMatrix();
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
   initAsteroids();

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
