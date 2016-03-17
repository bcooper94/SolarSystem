#version 330 core

uniform sampler2D Texture;

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

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

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

   texColor = texture(Texture, vTexCoord);

   vec3 preMappedColor = texColor.rgb * baseColor;

   float brightness = (preMappedColor.r + preMappedColor.g + preMappedColor.b) / 3;

   float gamma = 1.2;
   // Reinhard tone mapping
   vec3 mapped = preMappedColor / (preMappedColor + vec3(1.0));
   mapped = pow(mapped, vec3(1.0 / gamma));

	color = vec4(mapped, 1.0);

   if (brightness > 1) {
      brightColor = vec4(mapped, 1.0);
   }
   else {
      brightColor = vec4(0);
   }
}
 