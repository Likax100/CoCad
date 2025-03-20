#include "ResourceManager.h"
#include "CoCad_Utils.h"
#include <stdexcept>
#include <filesystem>

// ---- DEFINITIONS ---- //
std::map<std::string, Shader> ResourceManager::ShaderDict;
std::map<std::string, Texture2D> ResourceManager::TextureDict;
std::map<std::string, SpriteSheet> ResourceManager::SSDict;

ResourceType ResourceManager::r_type = RT_NONE;
std::string ResourceManager::def_T2D_path = "";
std::string ResourceManager::def_SH_path = "";
std::string ResourceManager::def_SS_path = "";

// ---- EXTERNAL FORWARD DECLARATIONS ---- //


// ---- IMPLEMENTATION ---- //
// TODO: Create just one function which can retrieve any resource given a flag (eg. RM_SHADER/RM_TEXTURE) or something of that sort
// TODO: Implement error codes when generating textures/shaders in case generation fails
void ResourceManager::GenShader(const char* vertext_sh_path, const char* frag_sh_path, std::string shader_name) {
  if (ShaderDict.count(shader_name) > 0) {
    std::string err = "ERROR-[RESOURCE_MANAGER::GenShader] Shader with name (" + shader_name + ") already exists.";    
    throw std::runtime_error(err);
  } else {
    Shader shader;
    shader.SetShaderName(shader_name);
    shader.LoadShaderFiles(vertext_sh_path, frag_sh_path);
    ShaderDict[shader_name] = shader;
  }
}

Shader ResourceManager::GetShader(std::string shader_name) {
  if (ShaderDict.count(shader_name) > 0) {
    return ShaderDict[shader_name]; 
  } else { 
    std::string err = "ERROR-[RESOURCE_MANAGER::GetShader] Shader Not Found (" + shader_name + ").";   
    throw std::runtime_error(err);   
  }
}


void ResourceManager::GenTexture(const char* image_path, std::string tex_name) {
  if (TextureDict.count(tex_name) > 0) {
    std::string err = "ERROR-[RESOURCE_MANAGER::GenTexture] Texture with name (" + tex_name + ") already exists.";
    throw std::runtime_error(err);
  } else {
    Texture2D tex(image_path);
    TextureDict[tex_name] = tex; 
  }
}

Texture2D& ResourceManager::GetTexture(std::string tex_name) {
  if (TextureDict.count(tex_name) > 0) {
    return TextureDict[tex_name];
  } else {
    std::string err = "ERROR-[RESOURCE_MANAGER::GetTexture] Texture Not Found (" + tex_name + ").";
    throw std::runtime_error(err);
  }
}

void ResourceManager::GenSpriteSheet(const char* sprite_sheet_path, int tiles_x, int tiles_y, std::string ss_name) {
  if (SSDict.count(ss_name) > 0) {
    std::string err = "ERROR-[RESOURCE_MANAGER::GenSpriteSheet] SpriteSheet with name (" + ss_name + ") already exists.";
    throw std::runtime_error(err);
  } else {
    SpriteSheet ss = SpriteSheet(sprite_sheet_path, tiles_x, tiles_y);
    SSDict[ss_name] = ss;
  }
}

SpriteSheet ResourceManager::GetSpriteSheet(std::string ss_name) {
  if (SSDict.count(ss_name) > 0) {
    return SSDict[ss_name];
  } else {
    std::string err = "ERROR-[RESOURCE_MANAGER::GetSpriteSheet] SpriteSheet Not Found (" + ss_name + ").";
    throw std::runtime_error(err);
  }
}

// NOTE: Bulk resource loading from file
// TODO: Improve this method, make it more efficient and less "if-prone"
// TODO: Add format error checking and validification of data
void ResourceManager::LoadResourcesFromFile(const char* resource_file_path) {
  std::filesystem::path file_path(resource_file_path);
  std::string ext = file_path.extension().string();

  if (!ext.empty() && ext == ".rsrc") {
    std::vector<std::string> rsrc_file_lines = FileUtil::ReadFileLines(resource_file_path);
    
    if (rsrc_file_lines[0] == "#resource_file") {
      
      for (int l = 1; l < rsrc_file_lines.size(); l++) {
        
        if (rsrc_file_lines[l] == "") { continue; }
        
        // Split into tokens and process depending on mode
        std::vector<std::string> tokens;
        size_t pos = 0;
        size_t delimiterPos;

        while ((delimiterPos = rsrc_file_lines[l].find(' ', pos)) != std::string::npos) {
          if (delimiterPos > pos) {
            tokens.push_back(rsrc_file_lines[l].substr(pos, delimiterPos - pos));
          }
          pos = delimiterPos + 1; // Move past the delimiter
        }
        // Add the last token
        if (pos < rsrc_file_lines[l].size()) { tokens.push_back(rsrc_file_lines[l].substr(pos)); }
      
        // PROCESSING TOKENS:
        // Setting Modes:
        if(tokens[0] == "::TEXTURE2D") { r_type = RT_TEXTURE2D; continue; }
        else if(tokens[0] == "::SHADER") { r_type = RT_SHADER; continue; }
        else if(tokens[0] == "::SPRITESHEET") { r_type = RT_SPRITESHEET; continue; } 
        else if(tokens[0] == "~TEXTURE2D_DEF_PATH") { def_T2D_path = tokens[1]; continue; } 
        else if(tokens[0] == "~SHADER_DEF_PATH") { def_SH_path = tokens[1]; continue; }
        else if(tokens[0] == "~SPRITESHEET_DEF_PATH") { def_SS_path = tokens[1]; continue; }

        // Importing Resources
        std::string path;
        int limit = (r_type != RT_SPRITESHEET) ? 2 : 4; 
        std::string name = (tokens.size() >= limit) ? tokens[0] : tokens.back();

        if(r_type == RT_TEXTURE2D && tokens.size() >= 1) {
          path = def_T2D_path + tokens[0];
          ResourceManager::GenTexture(path.c_str(), name); }

        else if(r_type == RT_SHADER && tokens.size() >= 1) {
          path = def_SH_path + tokens[0];
          const char* vert_path = (path + ".vs").c_str();
          const char* frag_path = (path + ".fs").c_str();
          std::cout << "V: " << vert_path << ", F: " << frag_path << std::endl;
          ResourceManager::GenShader(vert_path, frag_path, name); }
        
        else if(r_type == RT_SPRITESHEET && tokens.size() >= 3) {
          //path = def_SS_path + tokens[0];
          //ResourceManager::GenSpriteSheet(path.c_str(), std::stoi(tokens[1]), std::stoi(tokens[2]), name);
          std::cout << "Got Here! " << tokens[0] << std::endl;        
        }

      }
    } else { std::cout << "ERROR-[RESOURCE_MANAGER::LoadResourcesFromFile] Invalid File Header, use (#resource_file) if referring to resource files from OSLib." << std::endl; }
  } else {  
    std::cout << "ERROR-[RESOURCE_MANAGER::LoadResourcesFromFile] Invalid File Type (must be .rsrc)." << std::endl;
  }
}



