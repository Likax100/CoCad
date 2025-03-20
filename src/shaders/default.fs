#version 460 core
in vec2 outUVCoordinate;
out vec4 color;

uniform sampler2D spriteImage;
uniform vec3 v3_tint;
uniform float img_alpha;

void main() {
  color = vec4(v3_tint, img_alpha) * texture(spriteImage, outUVCoordinate);
}
