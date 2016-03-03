#version 330 core

uniform sampler2D SunTexture;

in vec3 fragNor;
in vec3 matAmb;
in vec3 lightDir;
in vec3 lightC;
in vec3 matDiff;
in vec3 matSpec;
in vec3 toCam;
in float specularP;
in vec3 atten;
in float dist;
in vec2 vTexCoord;

out vec4 color;

void main()
{
   vec3 baseColor;
   vec4 texColor;
   vec3 halfway = normalize(toCam + lightDir),
      normalizedNorm = normalize(fragNor),
      normalizedLight = normalize(lightDir);
   float lightAngle = dot(normalizedNorm, normalizedLight),
      halfwayAngle = dot(normalizedNorm, halfway),
      distance = length(lightDir);

   vec3 specularLight = pow(max(0, halfwayAngle), specularP) * lightC * matSpec,
      matDiffight = max(0, lightAngle) * lightC * matDiff;

   baseColor = matAmb + (specularLight + matDiffight)
      / (atten.x + atten.y * distance + atten.z * distance * distance);

   texColor = texture(SunTexture, vTexCoord);

	color = vec4(texColor.rgb * baseColor, 1.0);
   // color = vec4(texColor.r, texColor.g, texColor.b, 1.0);
}
 