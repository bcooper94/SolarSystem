#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
// layout(location = 3) in vec2 brightBuf;
uniform mat4 P;
uniform mat4 M;
uniform vec2 windowSize;

out vec3 HDRColor;
out vec2 vTexCoord;
out vec3 fragNor;
out vec3 lightDirection;

void main() {

	vec3 lightDir = vec3(1, 1, 1);
   vec4 vPosition;
   vec3 position = vertPos;

   if (windowSize.x > windowSize.y) {
      position.x *= windowSize.x / windowSize.y;
   }
   else {
      position.y *= windowSize.y / windowSize.x;
   }

   /* First model transforms */
   gl_Position = P * M * vec4(position.xyz, 1.0);

   fragNor = (M * vec4(vertNor, 0.0)).xyz;
   /* a color that could be blended - or be shading */
   HDRColor = vec3(max(dot(fragNor, normalize(lightDir)), 0));
   /* pass through the texture coordinates to be interpolated */
   vTexCoord = vertTex;
   lightDirection = lightDir;
}
