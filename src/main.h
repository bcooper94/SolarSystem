#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

#define STATIC_NORMALS 0
#define MOVING_NORMALS 1
#define GOURAUD 0
#define FULL_PHONG 1
#define SHADED_COLORING 0
#define NORMAL_COLORING 1

using namespace std;

static float degreesToRad(float degrees);

static void error_callback(int error, const char *description);

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void mouse_callback(GLFWwindow *window, int button, int action, int mods);

static void resize_callback(GLFWwindow *window, int width, int height);

static void init();

static void initGeom();

static void initPlanets();

static void initAsteroids();

static void setupDepthBuffer();

static void setupRenderTexture();

static void setupPingPongFBO();

static unsigned int createTexture(int width, int height, bool isDepth);

static void setupQuadBuffer();

static void drawBackground(shared_ptr<MatrixStack>& P);

static void drawAsteroids(shared_ptr<MatrixStack>& P, shared_ptr<MatrixStack> view);

static void drawQuad(shared_ptr<Program>& prog, int width, int height, int renderedTexture);

static void render();

#endif