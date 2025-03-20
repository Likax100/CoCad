#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Shaders.h"

#define MDL_PURE 0
#define MDL_REDUNDANT 1

class Model {
  public:
    Model();
    ~Model();
    
    void RenderModel(Shader& shader, bool mode = MDL_REDUNDANT);
    void LoadDataIntoBuffer();

    std::vector<float> unq_vertices;
    std::vector<float> unq_face_normals;
    std::vector<unsigned int> faces;
    std::vector<unsigned int> edges;

    std::vector<float> attrib_vertices;
    std::vector<float> attrib_edges;
    
    std::string mdl_name;
    std::string mdl_path;
    
    glm::mat4 mdl_matrix;

  private:
    unsigned int mdl_VAO, mdl_VBO, mdl_VBO_red, mdl_EBO;
    unsigned int mdl_eVAO, mdl_eVBO, mdl_eEBO;
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

