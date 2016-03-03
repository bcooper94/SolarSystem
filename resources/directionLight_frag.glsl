#version 330 core 
in vec3 fragNor;
in vec3 gouraudColor;
in vec3 matAmb;
in vec3 lightDir;
in vec3 lightC;
in vec3 meshC;
in vec3 matDiff;
in vec3 matSpec;
in vec3 toCam;
in float specularP;
in vec3 atten;
// in float dist;

out vec4 color;

void main()
{
   vec3 actualColor;
   // vec3 halfway = normalize(toCam + lightDir),
   vec3 normalizedNorm = normalize(fragNor);
   //    normalizedLight = normalize(lightDir);
   float lightAngle = dot(normalizedNorm, normalize(lightDir));
   //    halfwayAngle = dot(normalizedNorm, halfway),
   //    distance = length(lightDir);

   vec3 specularLight = pow(max(0, lightAngle), specularP) * lightC * matSpec,
      matDiffight = max(0, lightAngle) * lightC * matDiff;

   actualColor = matAmb * meshC + specularLight + matDiffight;
   color = vec4(actualColor, 1.0);
}
