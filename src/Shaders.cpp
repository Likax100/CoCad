#include "Shaders.h"
#include "CoCad_Utils.h"

Shader::Shader() { }
Shader::~Shader() { }

// Source File Loading and Shader Setup
void Shader::LoadShaderFiles(const char* v_file_path, const char* f_file_path) {
  //read source files and convert to const char*, then setup shaders
  std::string v_code = FileUtil::ReadFile(v_file_path);
  std::string f_code = FileUtil::ReadFile(f_file_path);

  const char* v_shader_source = v_code.c_str();
  const char* f_shader_source = f_code.c_str();

  //compiling shaders
  unsigned int v_shader, f_shader;
  
  int success;
  char error_log[512];

  //SHADER :: VERTEX
  v_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(v_shader, 1, &v_shader_source, NULL);
  glCompileShader(v_shader);

  glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(v_shader, 512, NULL, error_log);
    std::cout << "ERROR-[SHADER::VERTEX] Compilation Failed.\n" << error_log << std::endl;
  } else if(success && allStatusEnable) { std::cout << "STATUS-[SHADER::VERTEX] Compilation Successfull." << std::endl; }

  //SHADER :: FRAGMENT
  f_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(f_shader, 1, &f_shader_source, NULL);
  glCompileShader(f_shader);

  glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(f_shader, 512, NULL, error_log);
    std::cout << "ERROR-[SHADER::FRAG] Compilation Failed.\n" << error_log << std::endl;
  } else if(success && allStatusEnable) { std::cout << "STATUS-[SHADER::FRAG] Compilation Successfull." << std::endl; }

  //SHADER PROGRAM
  this->programID = glCreateProgram();
  glAttachShader(this->programID, v_shader);
  glAttachShader(this->programID, f_shader);
  glLinkProgram(this->programID);

  glGetProgramiv(this->programID, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(this->programID, 512, NULL, error_log);
    std::cout << "ERROR-[SHADER::PROG] Failed to link shaders to Program.\n" << error_log << std::endl;
  }

  //TODO: Display proper output depending on shader output
  int isValid;
  glValidateProgram(this->programID);

  glGetProgramiv(this->programID, GL_VALIDATE_STATUS, &isValid);
  if (isValid == true) { std::cout << "STATUS-[SHADER::PROG] Shader Program is Valid and Working: " << shaderName.c_str() << " - " << this->programID << "." << std::endl; }
  else { std::cout << "ERROR-[SHADER::PROG] Invalid Shader Program" << std::endl; }

  //SHADER CLEANUP - no need for v+f shaders after program linkage
  glDeleteShader(v_shader);
  glDeleteShader(f_shader);

}

void Shader::Use() const { glUseProgram(this->programID); }

void Shader::configStatus(bool statusEnable) {
  this->allStatusEnable = statusEnable;
}

void Shader::SetShaderName(std::string name) {
  this->shaderName = name;
}

//Uniform Function Definitions 
void Shader::setUInt(const char* uniform_name, int uniform_value) const {
  glUniform1i(glGetUniformLocation(this->programID, uniform_name), uniform_value);
}

void Shader::setUFloat(const char* uniform_name, float uniform_value) const {
  glUniform1f(glGetUniformLocation(this->programID, uniform_name), uniform_value);
}

void Shader::setUBool(const char* uniform_name, bool uniform_value) const {
  glUniform1i(glGetUniformLocation(this->programID, uniform_name), (int)uniform_value);
}

void Shader::setUMat4(const char* uniform_name, glm::mat4 &matrix) const {
  glUniformMatrix4fv(glGetUniformLocation(this->programID, uniform_name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUVec2(const char* uniform_name, glm::vec2 &vec) const {
  glUniform2fv(glGetUniformLocation(this->programID, uniform_name), 1, glm::value_ptr(vec));
}

void Shader::setUVec3(const char* uniform_name, glm::vec3 &vec) const {
  glUniform3fv(glGetUniformLocation(this->programID, uniform_name), 1, glm::value_ptr(vec));
}
