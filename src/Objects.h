#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <string>

#include "Texture2D.h"
#include "SpriteRenderer.h"

class Object2D {
  public:
  Object2D();
  Object2D(std::string sprite_path, glm::vec2 pos);
  ~Object2D();

  void RotateAxis(float angle, glm::vec3 axis);
  void Scale(float x, float y);

  void RenderObject(std::string renderer);
  
  glm::vec2 position;
  float alpha;
  glm::vec3 tint;
  glm::vec2 initial_scale;
  
  // STATIC STUFF
  static std::map<std::string, SpriteRenderer> ps_SpriteRenderers;
  static void AddSpriteRenderer(SpriteRenderer &ss, std::string name);

  private:
  Texture2D sprite_tex;
  glm::mat4 obj_mat;

};
