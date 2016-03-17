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
   vec2 texCoord = getTexCoords();
   vec3 texColor = texture(renderedTexture, texCoord).rgb,
      blurredColor = texture(brightTexture, texCoord).rgb,
      preMappedColor = texColor + blurredColor;

   float gamma = 1.2;
   // Reinhard tone mapping
   vec3 mapped = preMappedColor / (preMappedColor + vec3(1.0));
   mapped = pow(mapped, vec3(1.0 / gamma));

   color = vec4(mapped, 1.0);
}