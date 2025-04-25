#pragma once
#include "Window.h"
//#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "CoCad_Colors.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "Editor.h"
#include "CoCad_Utils.h"
#include "CoCad_UI.h"
#include "CoCad_Client.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_opengl3_loader.h"
#include "IconsFA.h"

#define GLFW_MOD_NONE 0

typedef struct {
  std::string username;
  std::string pass;
  int client_ID;
  bool is_host;
} User;

typedef struct {
  std::string user;
  std::string content;
} ChatMessage;

typedef struct {
  bool currently_in_session;
  int session_host;
  bool host_exists;
} SessionInfo;

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
  void SetupImGuiStyle();
  void AdjustScalingFromDPI();
  void SetupGrid(bool regen = false);
  void RenderGrid();
  void ProcessClient();

  // -------------------- VARS ------------------- //
  std::map<int, bool> keymap;
  int active_keymod = GLFW_MOD_NONE;
  std::map<std::string, glm::vec3> config_data;
  double mouse_x, mouse_y;
  bool mouse_click_handled = false;

  // networking stuff 
  CoCadClient cc_client;
  User user;
  SessionInfo sesh_info;

  // shader and rendering vars
  Shader glb_shader_3D;
  Shader glb_shader_vert;
  Shader glb_shader_baser;

  float glb_fov = 50.0f;
  const float glb_NEAR = 0.1f;
  const float glb_FAR = 500.0f;
  glm::mat4 glb_persp_proj;
  glm::mat4 glb_view_matrix;
 
  unsigned int grid_VAO, grid_VBO;
  glm::mat4 grid_model_mat = glm::mat4(1.0f);
  unsigned int grid_vertex_count;
  int grid_size = 20;
  float grid_unit_size = 0.5f;
  int grid_size_prev = grid_size;
  float grid_unit_size_prev = grid_size_prev;
  float half_g_length;
  float mvd_size = 0.01;
  float mvd_sel_radius_fact = 8.0f;

  Camera camera;
  bool isDragging = false;
  double drag_loc_start_x, drag_loc_start_y;
  double drag_loc_end_x, drag_loc_end_y;
  float light_intensity = 0.7f;

  ImGuiIO* imgui_io; 
  ImGuiStyle* imgui_style;
  ImFont* im_font_main;
  ImFont* im_font_icon;
  std::map<std::string, glm::vec2> window_locs;
  std::map<std::string, glm::vec2> window_sizes;
  ImVec4 bg_col;
  ImVec4 accent_col;
  ImVec4 i_mdl_color;
  ImVec4 i_dvert_color;
  ImVec4 i_svert_color;

  // operational vars
  double start_loc_x, start_loc_y;
  bool still_held = false;
  bool axis_locked[3] = {true, false, false}; 

  Model mdl;
  std::string mdl_path;
  std::string login_username;
  std::string login_password;
  bool logged_in = false;
  bool model_loaded = false;
  bool chat_enabled = false;
 
  // keep track of messages here?

}; 
