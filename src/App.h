#pragma once
#include "Window.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "Colors.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "CoCad_Utils.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_opengl3_loader.h"

class App : public Window {
  
  public:
  App(int window_width, int window_height, const char* window_title);
  virtual ~App();

  protected:
  void InitWindow() override;
  void Update() override;
  void Render() override; 
  void CleanWindow() override;
  
  void MouseButtonInput(int button, int action, int mods) override;
  void KeyboardInput(int key, int scancode, int action, int mods) override;
  void MouseScrollInput(double xoffset, double yoffset) override;

  int fb_width, fb_height;
  
  private:

  // -------------------- METHODS ------------------- //
  void SetupProgramConfig();
  void SetupGrid();
  void RenderGrid();

  // -------------------- VARS ------------------- //
  std::map<int, bool> keymap;
  std::map<std::string, glm::vec3> config_data;
  
  Shader glb_shader_3D;
  float glb_fov = 50.0f;
  const float glb_NEAR = 0.1f;
  const float glb_FAR = 500.0f;
  glm::mat4 glb_persp_proj;
  glm::mat4 glb_view_matrix;
 
  unsigned int grid_VAO, grid_VBO, grid_EBO;
  //glm::vec3 grid_line_color = glm::vec3(0.9f, 0.9f, 0.9f);
  glm::mat4 grid_model_mat = glm::mat4(1.0f);
  unsigned int grid_vertex_count;
  unsigned int grid_size = 30;
  float grid_unit_size = 0.5f;
  float half_g_length;

  Camera camera;
  bool isDragging = false;
  double drag_loc_start_x, drag_loc_start_y;
  double drag_loc_end_x, drag_loc_end_y;
  float sensitivity = 5.0f;

  ImGuiIO* imgui_io; 
  std::map<std::string, glm::vec2> window_locs;
  std::map<std::string, glm::vec2> window_sizes;
  float def_colorp_color[3] = {0.8f, 0.8f, 0.8f};

  Model kube;
}; 
