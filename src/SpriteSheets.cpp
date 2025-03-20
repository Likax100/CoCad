#include "SpriteSheets.h"

SpriteSheet::SpriteSheet() { } 

SpriteSheet::SpriteSheet(const char* sprite_sheet_path, int tiles_x, int tiles_y) {
  this->tiles_x = tiles_x;
  this->tiles_y = tiles_y;
  this->tile_count = tiles_x * tiles_y; 
  this->sheet = Texture2D(sprite_sheet_path);
  
  //calculate size of "uv tile"
  this->tile_size_x = 1.0f / (float)tiles_x;
  this->tile_size_y = 1.0f / (float)tiles_y;

  //calculate size of each "img tile"
  this->tile_img_size_x = sheet.width / tiles_x;
  this->tile_img_size_y = sheet.height / tiles_y;
}

SpriteSheet::~SpriteSheet() { }

int SpriteSheet::GetTileCount() { return this->tile_count; }
glm::vec2 SpriteSheet::GetDimensions() { return glm::vec2(this->tiles_x, this->tiles_y); }
glm::vec2 SpriteSheet::GetTileImgSize() { return glm::vec2(tile_img_size_x, tile_img_size_y); }


glm::vec4 SpriteSheet::GetTileUV(int tile_loc_x, int tile_loc_y) {
  float offset_x = tile_loc_x * this->tile_size_x;
  float offset_y = tile_loc_y * this->tile_size_y;
  return glm::vec4(offset_x, offset_y, this->tile_size_x, this->tile_size_y);
}

glm::vec4 SpriteSheet::GetTileUV(int tile_ID) {
  float offset_x = (tile_ID % this->tiles_x) * this->tile_size_x;
  float offset_y = (int)(((float)tile_ID / this->tile_count) * this->tiles_y) * this->tile_size_y;
  return glm::vec4(offset_x, offset_y, this->tile_size_x, this->tile_size_y);
}



