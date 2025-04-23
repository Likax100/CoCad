#pragma once

#include <iostream>
#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shaders.h"
#include "Texture2D.h"
#include "SpriteSheets.h"

class SpriteRenderer {
  public:
  SpriteRenderer();
  SpriteRenderer(Shader &shader);
  ~SpriteRenderer();

  void Render(Texture2D &tex, glm::vec2 pos, glm::vec2 scale = glm::vec2(1.0f, 1.0f), float rotate = 0.0f, glm::vec3 tint = glm::vec3(1.0f), float alpha = 1.0f);
  void Render(SpriteSheet sheet, int tile_ID, glm::vec2 pos, glm::vec2 scale = glm::vec2(1.0f, 1.0f), glm::vec3 tint = glm::vec3(1.0f), float alpha = 1.0f);
  void RenderRaw(Texture2D &tex, glm::mat4 model_matrix, float sprite_alpha, glm::vec3 tint_color);

  void EnableScaler(bool enable, float scale = 1.0f);

  void SetLayer(std::string layername); 
  void GenLayers(int count);

  private:
  Shader shader; 
  unsigned int qvao, vbo_1;
  float r_scale = 1.0f;

  std::map<std::string, float> layers;
  float current_layer = 0.0f;
  
};
