#include "Colors.h"

glm::vec3 Color::RGB(int r, int g, int b) {
  float r_norm = (float)r / 255.0f;
  float g_norm = (float)g / 255.0f;
  float b_norm = (float)b / 255.0f; 
  return glm::vec3(r_norm, g_norm, b_norm);
}
