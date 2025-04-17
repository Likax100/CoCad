#version 460 core
layout (location = 0) in vec3 inVertexPos;

uniform mat4 m4_model;
uniform mat4 m4_proj;
uniform mat4 m4_view;

void main() {
  gl_Position = m4_proj * m4_view * m4_model * vec4(inVertexPos, 1.0f);
}
