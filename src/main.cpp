/* Lab 5 base code - transforms using local matrix functions 
   to be written by students - 
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include "main.h"
#include "Texture.h"
#include "Planet.h"

using namespace std;
using namespace Eigen;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "resources/"; // Where the resources are loaded from
// string objFilename;
shared_ptr<Program> prog;
shared_ptr<Program> backgroundProg;
map<string, shared_ptr<Shape>> shapes;

vector<Planet> planets;
Planet *sun;

int g_width, g_height;
float objectRotation, lightXTranslate, cameraBeta, cameraAlpha;
int gMat = 0;
bool mouseDown = false;
double xPosition, yPosition;
Vector3f eye, upDirection, lookAtPoint;
bool mouseActive = true;

int g_GiboLen;

double curTime;

//global data for ground plane
GLuint BackgroundBuffObj, BackgroundNorBuffObj, BackgroundTexBuffObj, BackIndexBuffObj;

Texture starTexture;
GLint h_starTexture;

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
   }
}

static void switchMouseActive(GLFWwindow *window) {
   if (mouseActive) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }
   else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      cout << "Reactivating mouse: " << mouseActive << endl;
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

static void initPlants() {

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

static void initMeshes() {
   shapes["bunny"] = initShape("bunny.obj");
   shapes["nefertiti"] = initShape("Nefertiti-10K.obj");
   shapes["cube"] = initShape("cube.obj");
   shapes["intactRobot"] = initShape("intact_robot.obj");
   shapes["sphere"] = initShape("smoothsphere.obj");
}

static void init()
{
   GLSL::checkVersion();

   shapes = map<string, shared_ptr<Shape>>();
   objectRotation = 0;
   lightXTranslate = -2;
   eye = Vector3f(0, 0, 0);
   lookAtPoint = Vector3f(0, 0, -1);
   upDirection = Vector3f(0, 1, 0);
   xPosition = 0;
   yPosition = 0;
   // Set background color.
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   // Enable z-buffer test.
   glEnable(GL_DEPTH_TEST);
   planets = vector<Planet>();

   initMeshes();

   // Initialize the GLSL program.
   prog = make_shared<Program>();
   prog->setVerbose(true);
   prog->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
   prog->init();
   prog->addUniform("P");
   prog->addUniform("V");
   prog->addUniform("M");
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

   starTexture.setFilename(RESOURCE_DIR + "starry_sky.bmp");
   starTexture.setUnit(0);
   starTexture.setName("StarTexture");
   starTexture.init();

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
   backgroundProg->addTexture(&starTexture);
}

static void draw(shared_ptr<Program>& prog,
      shared_ptr<Shape>& shape, shared_ptr<MatrixStack>& M) {
   float x = cos(cameraAlpha) * cos(cameraBeta),
         y = sin(cameraAlpha), z = cos(cameraAlpha) * cos(3.1415 / 2 - cameraBeta);

   lookAtPoint = Vector3f(x, y, z) + eye;
   auto view = make_shared<MatrixStack>();
   view->pushMatrix();
   view->lookAt(eye, lookAtPoint, upDirection);

   glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
   glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, view->topMatrix().data());
   glUniform3f(prog->getUniform("lightPos"), 0, 0, -6);
   // glUniform3f(prog->getUniform("lightDirection"), -0.2, 0.2, 1);
   glUniform3f(prog->getUniform("lightColor"), 1, 1, 1);
   glUniform3f(prog->getUniform("attenuation"), 1, 0, 0);
   shape->draw(prog);

   view->popMatrix();
}

static void initPlanets() {
   sun = new Planet(1.0, 1.0, 0, 0, 1, Vector3f(0, 0, -6));

   planets.push_back(Planet(1.0, 2.0, 3.0, 3.0, 10.0, Vector3f(0, 0, -6)));
   planets.push_back(Planet(1.0, 2.0, 6.0, 6.0, 8.0, Vector3f(0, 0, -6)));
   planets.push_back(Planet(1.0, 2.0, 9.0, 9.0, 12.0, Vector3f(0, 0, -6)));
   planets.push_back(Planet(1.0, 2.0, 15.0, 11.0, 15, Vector3f(2, 0, -6)));
}

static void drawMesh(shared_ptr<Program>& prog,
      shared_ptr<Shape>& shape, shared_ptr<MatrixStack>& M) {
   draw(prog, shape, M);
}

static void placeMesh(shared_ptr<Program> prog, shared_ptr<Shape>& shape, const Vector3f& position,
   const Vector3f& scale, float rotation, const Vector3f& rotAxis, int material) {
   shared_ptr<MatrixStack> M = make_shared<MatrixStack>();

   M->pushMatrix();
   SetMaterial(material);
   M->translate(position);
   M->rotate(rotation, rotAxis);
   M->scale(scale);
   draw(prog, shape, M);
}

static void drawPlanets() {
   auto M = make_shared<MatrixStack>();

   M->pushMatrix();
   M->loadIdentity();

   SetMaterial(8);
   M->pushMatrix();
   M->translate(sun->getLocation(curTime));
   drawMesh(prog, shapes["sphere"], M);
   M->popMatrix();

   SetMaterial(2);

   for (size_t index = 0; index < planets.size(); index++) {
      M->pushMatrix();
      M->translate(planets[index].getLocation(curTime));
      drawMesh(prog, shapes["sphere"], M);
      M->popMatrix();
   }

   M->popMatrix();
}

static void render()
{
	// Get current frame buffer size.
	int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   glViewport(0, 0, width, height);

   curTime = glfwGetTime();
   cout << "Time: " << curTime << endl;

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;

   // Create the matrix stacks - please leave these alone for now
   auto P = make_shared<MatrixStack>();
   auto M = make_shared<MatrixStack>();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

   glDisable(GL_DEPTH_TEST);
   drawBackground(P);

   glEnable(GL_DEPTH_TEST);
	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());

   drawPlanets();

	prog->unbind();

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
