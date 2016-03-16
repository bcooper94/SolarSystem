#version 330 core

layout(location = 0) in vec4 vertPos;
// layout(location = 1) in vec2 vertTex;

uniform int windowWidth;
uniform int windowHeight;

flat out int wWidth;
flat out int wHeight;

// out vec2 texCoord;

void main() {
   gl_Position = vertPos;
   // texCoord = vertTex;
   wWidth = windowWidth;
   wHeight = windowHeight;
}