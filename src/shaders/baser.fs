#version 460 core
out vec4 frag_color;

in vec3 v3_vertexColor;
uniform float alpha;

void main() {
  float a = clamp(alpha, 0.0, 1.0);
  frag_color = vec4(v3_vertexColor.rgb, a); 
}
