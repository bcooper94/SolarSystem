#version 330 core

uniform sampler2D renderedTexture;
uniform sampler2D brightTexture;

// in vec2 texCoord;
flat in int wWidth;
flat in int wHeight;

out vec4 color;

vec2 getTexCoords() {
   return vec2(gl_FragCoord.x / wWidth, gl_FragCoord.y / wHeight);
}

void main() {
   // color = vec4(1, 0, 1, 1);
   vec2 texCoord = getTexCoords();
   vec4 texColor = texture(renderedTexture, texCoord),
      blurredColor = texture(brightTexture, texCoord);
   color = texColor + blurredColor;

   float brightness = (color.r + color.g + color.b) / 3;

   // if (brightness < 1) {
   //    discard;
   // }
   // color = vec4(texCoord, 0, 1);
   // color = vec4(0, 1, 0, 1);
   // float grayScale = (color.r + color.g + color.b) / 3;
   // color = vec4(vec3(grayScale), 1);
}