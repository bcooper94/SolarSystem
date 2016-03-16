#version 330 core

uniform sampler2D renderedTexture;

in vec2 texCoord;

out vec4 color;

vec2 getTexCoords() {
   return gl_FragCoord.xy * 0.5 + vec2(0.5, 0.5);
}

void main() {
   // color = vec4(1, 0, 1, 1);
   // vec2 texCoord = getTexCoords();
   color = texture(renderedTexture, texCoord);
   // color = vec4(0, 1, 0, 1);
   // float grayScale = (color.r + color.g + color.b) / 3;
   // color = vec4(vec3(grayScale), 1);
}