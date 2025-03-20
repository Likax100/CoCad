#pragma once

#include <iostream>
#include <glm/glm.hpp>

#include "Texture2D.h"

class SpriteSheet {
  public:
  SpriteSheet();
  SpriteSheet(const char* sprite_sheet_path, int tiles_x, int tiles_y);
  ~SpriteSheet();

  glm::vec4 GetTileUV(int tile_loc_x, int tile_loc_y);
  glm::vec4 GetTileUV(int tile_ID);
  int GetTileCount();
  glm::vec2 GetDimensions();
  glm::vec2 GetTileImgSize();

  Texture2D sheet;
  
  private:
  float tile_size_x, tile_size_y;
  int tiles_x, tiles_y;
  int tile_count;

  int tile_img_size_x, tile_img_size_y;
};

