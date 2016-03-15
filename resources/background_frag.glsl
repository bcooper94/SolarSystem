#version 330 core
uniform sampler2D StarTexture;

in vec2 vTexCoord;
in vec3 HDRColor;

out vec4 Outcolor;

vec3 blur(){
   float dx = 1.0f / 1024;
   float dy = 1.0f / 768;
   vec3 sum = vec3(0,0,0);

   for(int i = -5; i < 5; i++) {
      for(int j = -5; j < 5; j++) {
         sum += texture(StarTexture, vTexCoord + vec2(i * dx, j  * dy)).xyz;
      }
   }

   return sum / 50;
}

void main() {
  vec4 BaseColor = vec4(HDRColor.x, HDRColor.y, HDRColor.z, 1);
  vec4 texColor2 = texture(StarTexture, vTexCoord);
  float brightness = BaseColor.r + BaseColor.g + BaseColor.b / 3;
 
   // if (brightness > 1.1) {
   //    Outcolor = vec4(blur(), 1);
   // }
   // else {
      Outcolor = vec4(texColor2.r * HDRColor.r, texColor2.g * HDRColor.g, texColor2.b * HDRColor.b, 1);
   // }
}
