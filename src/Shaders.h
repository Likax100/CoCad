#pragma once
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
  Shader();
  ~Shader();

  unsigned int programID;
  
  void LoadShaderFiles(const char* v_file_path, const char* f_file_path);
  void LoadShaderFile(const char* c_file_path); //TODO: Implement combined shader reading feature
  
  void Use() const;
  void configStatus(bool statusEnable);
  void SetShaderName(std::string name); 

  // Uniform Setting Functions - TODO: Add feautre to AUTO use shader before setting if not used already
  void setUInt(const char* uniform_name, int uniform_value) const; 
  void setUFloat(const char* uniform_name, float uniform_value) const;
  void setUBool(const char* uniform_name, bool uniform_value) const;

  void setUMat4(const char* uniform_name, glm::mat4 &matrix) const;

  void setUVec2(const char* uniform_name, glm::vec2 &vec) const;
  void setUVec3(const char* uniform_name, glm::vec3 &vec) const;
  void setUVec4(const char* uniform_name, glm::vec4 &vec) const;

private:
  std::string shaderName;
  bool allStatusEnable = false;
};


