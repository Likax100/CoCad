#version 460 core
layout (location = 0) in vec3 inVertexPos;
layout (location = 1) in mat4 inInstanceMat;
layout (location = 5) in vec3 inInstanceColor;

uniform mat4 m4_view;
uniform mat4 m4_proj;

out vec3 v3_color;

void main() {
  v3_color = inInstanceColor;
  gl_Position = m4_proj * m4_view * inInstanceMat * vec4(inVertexPos, 1.0);  
}
