#version 460 core
out vec4 frag_color;

uniform vec3 v3_vertexColor;

void main() {
  frag_color = vec4(v3_vertexColor.rgb, 1.0); 
}
