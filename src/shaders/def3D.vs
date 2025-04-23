#version 460 core
layout (location = 0) in vec3 inVertexPosition;
layout (location = 1) in vec3 inVertexNormal;
layout (location = 2) in vec3 inVertexColor;

out vec3 inNormal;
out vec3 inColor;

uniform mat4 m4_model;
uniform mat4 m4_proj;
uniform mat4 m4_view;

void main() {
  inNormal = inVertexNormal;
  inColor = inVertexColor;
  gl_Position = m4_proj * m4_view * m4_model * vec4(inVertexPosition, 1.0f);
}
