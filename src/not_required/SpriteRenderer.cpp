#include "SpriteRenderer.h"
#include "ShapeData.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtx/io.hpp>

SpriteRenderer::SpriteRenderer() { }

SpriteRenderer::SpriteRenderer(Shader &shader) {
  this->shader = shader;

  std::cout << "STATUS-[SPRITE_RENDERER::SpriteRenderer] Shader Attached Successfully." << std::endl;

  // SET UP QUAD DATA - TODO: Add buffer verification and validation
  unsigned int vbo_0;
 
  glGenVertexArrays(1, &this->qvao);
  glGenBuffers(1, &vbo_0);
  glGenBuffers(1, &this->vbo_1);
    
  glBindVertexArray(this->qvao);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo_0);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Quad::vertices2D), Quad::vertices2D, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Quad::uvCoords), Quad::uvCoords, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);  
  glBindVertexArray(0);
}

SpriteRenderer::~SpriteRenderer() {
  glDeleteVertexArrays(1, &this->qvao);
}

void SpriteRenderer::Render(Texture2D &tex, glm::vec2 pos, glm::vec2 scale, float rotate, glm::vec3 tint, float alpha) {
  this->shader.Use();
  
  // Matrix transformations are in reverse order:
  // Usual Ordor: Scale -> Rotate -> Translate
  // Hence we perform operations starting at Translate

  // Translate:
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(pos, 0.0f));

  // Rotations: Adjust anchor point first before rotation and bring back after
  if (rotate != 0.0f) {
    model = glm::translate(model, glm::vec3(0.5f * scale.x, 0.5f * scale.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * scale.x, -0.5f * scale.y, 0.0f));
  }
  // Scale
  model = glm::scale(model, glm::vec3(tex.width, tex.height, 1.0f)); //scale to original 
  model = glm::scale(model, glm::vec3(scale, 1.0f)); //custom scale

  if (r_scale != 1.0f) {
    model = glm::scale(model, glm::vec3(r_scale, r_scale, 1.0f)); //render size scale
  }

  // Set Shader Uniforms
  this->shader.setUMat4("m4_model", model);
  this->shader.setUVec3("v3_tint", tint);
  this->shader.setUFloat("z_layer", this->current_layer);
  this->shader.setUFloat("img_alpha", alpha);
  
  // Reset the vbo uv buffer when drawing regular sprites (since spritesheet modifies it)
  glBindVertexArray(this->qvao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Quad::uvCoords), Quad::uvCoords, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Draw Sprite
  tex.Use();  

  glBindVertexArray(this->qvao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void SpriteRenderer::Render(SpriteSheet sheet, int tile_ID, glm::vec2 pos, glm::vec2 scale, glm::vec3 tint, float alpha) {
  // Translate
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(pos, 0.0f));

  // Scale
  model = glm::scale(model, glm::vec3(sheet.GetTileImgSize(), 1.0f));
  model = glm::scale(model, glm::vec3(scale, 1.0f));

  if (r_scale != 1.0f) {
    model = glm::scale(model, glm::vec3(r_scale, r_scale, 1.0f));
  }

  // Set Shader Uniforms
  this->shader.setUMat4("m4_model", model);
  this->shader.setUVec3("v3_tint", tint);
  this->shader.setUFloat("z_layer", this->current_layer);
  this->shader.setUFloat("img_alpha", alpha);
  
  // Modify UV Buffer passing the correct Tile UV Data
  glm::vec4 tile_uv = sheet.GetTileUV(tile_ID);

  float dynamic_uv[12] = {
    tile_uv.x, tile_uv.y + tile_uv.w,
    tile_uv.x + tile_uv.z, tile_uv.y,
    tile_uv.x, tile_uv.y,
    
    tile_uv.x, tile_uv.y + tile_uv.w,
    tile_uv.x + tile_uv.z, tile_uv.y + tile_uv.w,
    tile_uv.x + tile_uv.z, tile_uv.y
  };

  glBindVertexArray(this->qvao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(dynamic_uv), dynamic_uv, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Draw Tile
  sheet.sheet.Use();
  
  glBindVertexArray(this->qvao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void SpriteRenderer::RenderRaw(Texture2D &tex, glm::mat4 model_matrix, float sprite_alpha, glm::vec3 tint_color) {
  if (r_scale != 1.0f) {
    model_matrix = glm::scale(model_matrix, glm::vec3(r_scale, r_scale, 1.0f));
  }

  this->shader.Use();
  this->shader.setUMat4("m4_model", model_matrix);
  this->shader.setUVec3("v3_tint", tint_color);
  this->shader.setUFloat("z_layer", this->current_layer);
  this->shader.setUFloat("img_alpha", sprite_alpha);
  
  glBindVertexArray(this->qvao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Quad::uvCoords), Quad::uvCoords, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  tex.Use();  

  glBindVertexArray(this->qvao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void SpriteRenderer::EnableScaler(bool enable, float scale) {
  if (enable == true) { r_scale = scale; }
  else { r_scale = 1.0f; }
}

void SpriteRenderer::SetLayer(std::string layer_name) {
  if (this->layers.count(layer_name) > 0) {
    this->current_layer = this->layers[layer_name];
  } else { std::cout << "ERROR-[SPRITE_RENDERER::SetLayer] Layer Not Found." << std::endl; }
}

void SpriteRenderer::GenLayers(int count) {
  // NOTE: Arbitrary max layer count set to 32 layers (giving 0.05 as min distance between quads),
  // this max is required for now as quads too close to eachother may still struggle to render 
  // due to rounding errors and such. 

  this->layers.clear();

  if(count <= 32 && count > 1) {
    float distance = 1.6f / ((float)count - 1.0f);
    for (int i = 0; i < count; i++) {
      // TODO: Custom layer names??
      std::string layer_name = "Layer" + std::to_string(i);
      float layer_z_value = 0.8f - (distance * i);
      this->layers[layer_name] = layer_z_value;

      //std::cout << "Layer::" << layer_name << " = " << layer_z_value << std::endl;
    }
  } else { std::cout << "ERROR-[SPRITE_RENDERER::GenLayers] Incorrect Layer Count, must be between 2 - 32." << std::endl; }
}

