#version 330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec2 vertTex;

// out vec2 texCoord;

void main() {
   gl_Position = vertPos;
   // texCoord = vertTex;
}