#version 330 core

uniform sampler2D renderedTexture;
uniform int horizontal;
uniform vec2 blurDir;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

flat in int wWidth;
flat in int wHeight;

out vec4 color;

vec2 getTexCoords() {
   return vec2(gl_FragCoord.x / wWidth, gl_FragCoord.y / wHeight);
}

vec4 gaussianBlur2Pass(sampler2D texID, vec2 texel, float resolution, float radius, vec2 dir){
    vec4 sum = vec4(0.0);
    float blur = radius / resolution;
    const int kernel_size = 17;
    const int half_kernel_size = kernel_size / 2;
    float gauss_kernel[17] = float [] (
        0.00118045,
        0.0037082,
        0.00999995,
        0.02315,
        0.0460066,
        0.0784889,
        0.114951,
        0.144523,
        0.155983,
        0.144523,
        0.114951,
        0.0784889,
        0.0460066,
        0.02315,
        0.00999995,
        0.0037082,
        0.00118045
    );
    for (int i = -half_kernel_size; i <= half_kernel_size; i++){
        sum += texture(texID,
           vec2(texel.x - i*blur*dir.x, texel.y - i*blur*dir.y)) * gauss_kernel[i + half_kernel_size];
    }
    return sum;
}

void main() {
   vec2 texCoord = getTexCoords();
   color = texture(renderedTexture, texCoord);

   vec2 tex_offset = 5.0 / textureSize(renderedTexture, 0); // gets size of single texel
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
   // vec2 dir;

   // if (horizontal == 1) {
   //    dir = vec2(1, 1);
   // }
   // else {
   //    dir = vec2(0, 1);
   // }

   // vec3 result = gaussianBlur2Pass(renderedTexture, texCoord, 500, 3, dir).rgb;

   // color = gaussianBlur2Pass(renderedTexture, texCoord, 500, 3, blurDir);

   color = vec4(result, 1.0);
}