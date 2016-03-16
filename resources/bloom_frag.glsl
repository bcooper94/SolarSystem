#version 330 core

uniform sampler2D renderedTexture;
uniform int horizontal;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

flat in int wWidth;
flat in int wHeight;

out vec4 color;

vec2 getTexCoords() {
   return vec2(gl_FragCoord.x / 1024, gl_FragCoord.y / 768);
}

void main() {
   vec2 texCoord = getTexCoords();
   color = texture(renderedTexture, texCoord);

   float brightness = (color.r + color.g + color.b) / 3;

   // if (brightness < 1) {
   //    discard;
   // }
   // color = vec4(texCoord, 0, 1);
   // color = vec4(0, 1, 0, 1);
   // float grayScale = (color.r + color.g + color.b) / 3;
   // color = vec4(vec3(grayScale), 1);

   vec2 tex_offset = 1.0 / textureSize(renderedTexture, 0); // gets size of single texel
   vec3 result = texture(renderedTexture, texCoord).rgb * weight[0]; // current fragment's contribution

   if(horizontal == 1) {
      for(int i = 1; i < 5; ++i) {
         result += texture(renderedTexture, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
         result += texture(renderedTexture, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
      }
   }
   else {
      for(int i = 1; i < 5; ++i) {
         result += texture(renderedTexture, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
         result += texture(renderedTexture, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
      }
   }

   color = vec4(result, 1.0);
}