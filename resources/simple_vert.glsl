#version 330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float specularPower;
uniform vec3 attenuation;

out vec3 fragNor;
out vec3 matAmb;
out vec3 matDiff;
out vec3 matSpec;
out vec3 lightDir;
out vec3 lightC;
out vec3 toCam;
out float specularP;
out vec3 atten;
out float dist;
out vec2 vTexCoord;

void main()
{
   vec4 worldPos = M * vertPos;
   vec3 toCamera = -worldPos.xyz;
   float distance = length(lightPos - worldPos.xyz);

   fragNor = (M * vec4(vertNor, 0.0)).xyz;

	gl_Position = P * V * worldPos;
   matAmb = matAmbient;
   lightDir = lightPos - worldPos.xyz;
   lightC = lightColor;
   matDiff = matDiffuse;
   matSpec = matSpecular;
   toCam = toCamera;
   specularP = specularPower;
   atten = attenuation;
   dist = distance;
   vTexCoord = vertTex;
}
