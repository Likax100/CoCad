#pragma once
#include <iostream>
#include <string>
#include <map>

#include "Shaders.h"
#include "Texture2D.h"
#include "SpriteSheets.h"

enum ResourceType {
  RT_NONE,
  RT_TEXTURE2D,
  RT_SHADER,
  RT_SPRITESHEET
};

class ResourceManager {
  public:
  static void GenShader(const char* vertex_sh_path, const char* frag_sh_path, std::string shader_name);
  static Shader GetShader(std::string shader_name);
  
  static void GenTexture(const char* image_path, std::string tex_name);
  static Texture2D& GetTexture(std::string tex_name);

  static void GenSpriteSheet(const char* sprite_sheet_path, int tiles_x, int tiles_y, std::string ss_name);
  static SpriteSheet GetSpriteSheet(std::string ss_name);

  static void LoadResourcesFromFile(const char* resource_file_path);

  private:
  ResourceManager() { }
  ~ResourceManager() { }

  static std::map<std::string, Shader> ShaderDict;
  static std::map<std::string, Texture2D> TextureDict;
  static std::map<std::string, SpriteSheet> SSDict;

  static ResourceType r_type;
  static std::string def_T2D_path;
  static std::string def_SH_path;
  static std::string def_SS_path;

};
