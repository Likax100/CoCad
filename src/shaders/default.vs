#version 460 core
layout (location = 0) in vec2 inVertexPosition;
layout (location = 1) in vec2 inUVCoordinate;

out vec2 outUVCoordinate;

uniform float z_layer;
uniform mat4 m4_model;
uniform mat4 m4_proj;

void main() {
  outUVCoordinate = inUVCoordinate;
  gl_Position = m4_proj * m4_model * vec4(inVertexPosition, z_layer, 1.0);
} 
