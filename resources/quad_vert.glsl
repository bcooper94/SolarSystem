#version 330 core

layout(location = 0) in vec3 vertPos;

uniform mat4 P;
uniform mat4 MV;
uniform vec2 windowSize;
uniform vec2 windowCenter;

out vec2 wCenter;

void main() {
   vec3 position = vertPos;

   if (windowSize.x > windowSize.y) {
      position.x *= windowSize.x / windowSize.y;
   }
   else {
      position.y *= windowSize.y / windowSize.x;
   }

   gl_Position = P * MV * vec4(position, 1.0);
   wCenter = windowCenter;
}
