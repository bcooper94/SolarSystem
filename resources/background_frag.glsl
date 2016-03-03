#version 330 core
uniform sampler2D StarTexture;

in vec2 vTexCoord;
in vec3 HDRColor;

out vec4 Outcolor;

void main() {
  vec4 BaseColor = vec4(HDRColor.x, HDRColor.y, HDRColor.z, 1);
  vec4 texColor2 = texture(StarTexture, vTexCoord);
 
	Outcolor = vec4(texColor2.r * HDRColor.r, texColor2.g * HDRColor.g, texColor2.b * HDRColor.b, 1);
}
