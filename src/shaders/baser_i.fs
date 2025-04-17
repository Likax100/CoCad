#version 460 core
out vec4 frag_color;

in vec3 v3_color;
uniform float alpha;

void main() {
  frag_color = vec4(v3_color, alpha);
}

