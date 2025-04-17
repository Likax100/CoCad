#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Shaders.h"

class Model {
  public:
    Model();
    ~Model();
    
    void RenderModel(Shader& shader);
    void LoadDataIntoBuffer();

    std::vector<float> unq_vertices;
    std::vector<float> unq_face_normals;
    std::vector<unsigned int> faces;

    std::vector<float> attrib_vertices;
    
    std::string mdl_name;
    std::string mdl_path;
    
    glm::mat4 mdl_matrix;

  private:
    unsigned int mdl_VAO, mdl_VBO;
};

// NOTE: Wavefront Objects MUST be triangulated when exporting! Or there will
// be missing faces unfortunately :/
// POSSIBLE FIX: separate tri faces and quad faces, perform seperate draw calls
// for both types of rendering (aka use GL_QUADS and GL_TRIANGLES for certain faces)

class OBJLoader {
  public:
    static Model LoadModel(const char* file_path);

  private:
    static std::vector<int> vertex_normal_index;
};

class STLLoader {
  public:
    static Model LoadModel(const char* file_path);

  private:
};

