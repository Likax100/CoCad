#version 460 core
layout (location = 0) in vec3 inVertexPositionNR;
layout (location = 1) in vec3 inVertexPositionR;
layout (location = 2) in vec3 inVertexNormalR;

out vec3 inNormal;

uniform int ren_mode;
uniform mat4 m4_model;
uniform mat4 m4_proj;
uniform mat4 m4_view;

void main() {
  vec3 vertex_coord;
  if (ren_mode == 0) { vertex_coord = inVertexPositionNR; }
  else { vertex_coord = inVertexPositionR; }

  inNormal = inVertexNormalR;
  gl_Position = m4_proj * m4_view * m4_model * vec4(vertex_coord, 1.0f);
}
