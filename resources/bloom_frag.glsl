#version 330 core

uniform sampler2D renderedTexture;

in vec2 texCoord;

out vec4 color;

void main() {
   // color = vec4(1, 0, 1, 1);
   color = texture(renderedTexture, texCoord);
   // float grayScale = (color.r + color.g + color.b) / 3;
   // color = vec4(vec3(grayScale), 1);
}