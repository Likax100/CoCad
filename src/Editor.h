#pragma once

#include <iostream>
#include <set>
#include <utility>
#include <algorithm>
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

#define ray_epsilon 0.000001f

typedef struct {
  std::vector<float> og_vert_cpy;

  std::vector<float> unique_verts;
  std::vector<float> unique_face_normals;
  std::vector<unsigned int> face_indices;
  std::vector<unsigned int> normals_redundant;

  // Selection Logic
  std::vector<unsigned int> selected_verts;
  glm::vec3 sel_color;
  glm::vec3 desel_color;
} EditorRepr;

typedef struct {
  std::vector<float> glda_vertex_data;
  unsigned int render_VAO;
  unsigned int render_VBO;
  glm::vec3 mdl_color;
} RenderRepr;

typedef std::pair<int, int> Edge;

class Editor {
  public:
    static void SetUp(Camera* camera, float w, float h);
    static void SetSelectionMode(unsigned int mode);
    static unsigned int GetSelectionMode();
    static void SetMVDScalings(float def_scale, float vert_sel_radius);
    static void SetSelectionColors(glm::vec3 vert_sel_col, glm::vec3 vert_desel_col);

    static void GenerateRepr(Model& mdl); 
    static void SetUpMVD();
    static void RecalculateMVD();
    static void SetUpMED();
    static void ClearRepr();
   
    static void RecalculateNormals();
    static void CastRay(float mouse_x, float mouse_y, glm::mat4& view_mat, glm::mat4& proj_mat);
    static bool MollerTriangleIntersect(Ray r, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& ray_t, float& u, float& v);
    
    static void RenderVertOverlay(Shader& sh);
    static void UpdateVertOverlayInstanceScale(float adjust_val);
    static void RenderEdgeOverlay(Shader& sh);
    static void GenRenderBufferData();
    static void GlobalModelTransform();
    static void Render(Shader& sh);
    static void PassModKeyControl(bool state);

    static EditorRepr repr;
    static RenderRepr r_repr;
    static bool instance_data_updated;
    static bool instance_color_updated;
  
    static float med_sel_dist; // MED raycasting dist threshold
    static std::set<unsigned int> sel_unique_verts;
    static bool move_mode_active;
    static bool move_dat_update;
    static bool edge_data_updated;
    static std::vector<glm::vec3> mvd_c_positions;
    static std::vector<glm::vec3> og_mvdc_pos_cpy;

    static float mdl_glb_scale[4];
    static int mdl_glb_rot[4];
    static float mdl_glb_position[4];
    static bool mdl_glb_updated;

  private:
    // Model Vertex Display stuff
    static glm::vec3 mvd_glb_scale; 
    static float mvd_def_scale;
    static float mvd_sel_radius;
    static std::vector<glm::mat4> mvd_instance_data;
    static std::vector<glm::vec3> mvd_instance_colors;
    static unsigned int mvd_c_VAO, mvd_c_VBO;
    static unsigned int mvd_instance_VBO;
    static unsigned int mvd_color_VBO;

    // Model Edge Display Stuff
    static unsigned int med_VAO, med_VBO;
    static std::vector<float> med_edge_data;
    static std::set<Edge> med_unique_edges;
    static glm::mat4 edge_model_mat;

    // General vars
    static float s_width, s_height;
    static unsigned int sel_mode;
    static Camera* cam;
    static bool control_key_state; 

    // TESTING VARS
    static bool TEST;
};
