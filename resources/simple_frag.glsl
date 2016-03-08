#version 330 core

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
   float shine = specularP;
   vec3 actualColor;
   vec3 halfway = normalize(toCam + lightDir),
      normalizedNorm = normalize(fragNor),
      normalizedLight = normalize(lightDir);
   float lightAngle = dot(normalizedNorm, normalizedLight),
      halfwayAngle = dot(normalizedNorm, halfway),
      distance = length(lightDir);

   if (actualColor.b > (actualColor.r + actualColor.g + actualColor.b) / 3) {
      shine += 50;
   }

   vec3 specularLight = pow(max(0, halfwayAngle), shine) * lightC * matSpec,
      matDiffight = max(0, lightAngle) * lightC * matDiff;
   

   actualColor = matAmb + (specularLight + matDiffight)
      / (atten.x + atten.y * distance + atten.z * distance * distance);


	color = vec4(actualColor, 1.0);
}
