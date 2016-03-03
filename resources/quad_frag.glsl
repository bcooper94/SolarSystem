#version 330 core

in vec2 wCenter;

void main() {
   float colorScale = 1.0 - min(1.0, distance(wCenter, vec2(gl_FragCoord.x, gl_FragCoord.y)) / 400.0);
   gl_FragColor = vec4(0.5, 0, 0.69, 1.0) + colorScale * vec4(1, 1, 1, 1);
}
