#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

class Texture2D {
  public:
  Texture2D();
  Texture2D(const char* image_path);
  ~Texture2D();

  void Use(int tex_unit = 0);
  glm::vec2 GetSize();

  unsigned int ID;

  int width;
  int height;
  int channels;
};
