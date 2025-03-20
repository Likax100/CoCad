#include "Texture2D.h"

#include <iostream>
#include "../include/external/stb_image.h"

Texture2D::Texture2D() { }

Texture2D::Texture2D(const char* image_path) {
  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //NOTE: Not needed since we changed origin (0, 0) from BOT-R to TOP-R
  //stbi_set_flip_vertically_on_load(1); 
  unsigned char* data = stbi_load(image_path, &this->width, &this->height, &this->channels, 0);
  //std::cout << "[IMAGE INFO] " << image_path << " : w = " << w << ", h = " << h << ", chn = " << chns << std::endl;

  int IMG_TYPE = (channels == 4) ? GL_RGBA : GL_RGB;

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, IMG_TYPE, width, height, 0, IMG_TYPE, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    std::cout << "STATUS-[TEXTURE2D::Texture2D] Asset (" << image_path << ") Created as Texture2D" << std::endl;
  } else {
    std::cout << "ERROR-[TEXTURE2D::Texture2D] Failed to Load Image\n" << stbi_failure_reason() << std::endl;
  }
 
  stbi_image_free(data);
}

Texture2D::~Texture2D() { }

glm::vec2 Texture2D::GetSize() {
  return glm::vec2(width, height);
}

void Texture2D::Use(int tex_unit) {
  glActiveTexture(GL_TEXTURE0 + tex_unit);
  glBindTexture(GL_TEXTURE_2D, ID);
}
