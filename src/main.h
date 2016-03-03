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

static void SetMaterial(int i);

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void mouse_callback(GLFWwindow *window, int button, int action, int mods);

static void resize_callback(GLFWwindow *window, int width, int height);

static shared_ptr<Shape> initShape(string objFilename);

static void initMeshes();

static void init();

static void initGeom();

static void initPlanets();

static void draw(shared_ptr<Program>& prog, shared_ptr<Shape>& shape, shared_ptr<MatrixStack>& M);

static void drawMesh(shared_ptr<Program>& prog, shared_ptr<Shape>& shape, shared_ptr<MatrixStack>& M);

static void drawMeshes();

static void render();

#endif