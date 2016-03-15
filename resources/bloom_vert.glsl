#version 330 core

layout(location = 0) in vec2 vertTex;

out vec2 texCoord;

void main() {
   texCoord = vertTex;
}