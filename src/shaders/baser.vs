#version 460 core
layout (location = 0) in vec3 inVertexPos;
layout (location = 1) in vec3 inVertexColor;

uniform mat4 m4_model;
uniform mat4 m4_proj;
uniform mat4 m4_view;

out vec3 v3_vertexColor;

void main() {
  v3_vertexColor = inVertexColor;
  gl_Position = m4_proj * m4_view * m4_model * vec4(inVertexPos, 1.0f);
}
