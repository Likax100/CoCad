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
  
  // -------------------- VARS ------------------- //
  std::map<int, bool> keymap;
  std::map<std::string, glm::vec3> config_data;
  
  Shader glb_shader_3D;
  float glb_fov = 50.0f;
  const float glb_NEAR = 0.1f;
  const float glb_FAR = 500.0f;
  glm::mat4 glb_persp_proj;
  glm::mat4 glb_view_matrix;
  
  Camera camera;
  bool isDragging = false;
  double drag_loc_start_x, drag_loc_start_y;
  double drag_loc_end_x, drag_loc_end_y;
  float sensitivity = 1.5f;

  ImGuiIO* imgui_io; 
  std::map<std::string, glm::vec2> window_locs;
  std::map<std::string, glm::vec2> window_sizes;
  float def_colorp_color[3] = {0.8f, 0.8f, 0.8f};

  Model kube;
}; 
