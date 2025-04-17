#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "RayCasting.h"
#include "Shaders.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "CoCad_Utils.h"

#define COCAD_SELMODE_NONE -1
#define COCAD_SELMODE_VERT 0
#define COCAD_SELMODE_EDGE 1
#define COCAD_SELMODE_FACE 2

typedef struct {
  std::vector<float> unique_verts;
  std::vector<unsigned int> face_indices;
  std::vector<unsigned int> selected_verts;
  glm::vec3 sel_color;
  glm::vec3 desel_color;
} EditorRepr;

class Editor {
  public:
    static void SetUp(Camera* camera, float w, float h);
    static void SetSelectionMode(unsigned int mode);
    static unsigned int GetSelectionMode();
    static void GenerateRepr(Model& mdl); 
    static void CastRay(float mouse_x, float mouse_y, glm::mat4& view_mat, glm::mat4& proj_mat);
    static void RenderVertOverlay(Shader& sh);
    //static void UpdateVertOverlayInstancePos(std::vector<glm::mat4>& data);
    static void UpdateVertOverlayInstanceScale(float adjust_val);

    static EditorRepr repr;
  private:
    // Model Vertex Display stuff
    static glm::vec3 mvd_glb_scale; 
    static std::vector<glm::vec3> mvd_c_positions;
    static std::vector<glm::mat4> mvd_instance_data;
    static std::vector<glm::vec3> mvd_instance_colors;
    static unsigned int mvd_c_VAO, mvd_c_VBO;
    static unsigned int mvd_instance_VBO;
    static unsigned int mvd_color_VBO;
    static bool instance_data_updated;
    static bool instance_color_updated;

    // General vars
    static float s_width, s_height;
    static unsigned int sel_mode;
    static Camera* cam;
};
