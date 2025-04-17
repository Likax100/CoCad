#include "App.h"

#include <glm/glm.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#define FBO_WIDTH w_width * monitor_scale_x
#define FBO_HEIGHT w_height * monitor_scale_y

// ============================================================= //
//                  --- INIT/SETUP SECTION ---
// ============================================================= //
App::App(int window_width, int window_height, const char* window_title) :
  Window(window_width, window_height, window_title) {
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    
    if (glfwRawMouseMotionSupported()) {
      glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
}

App::~App() { }

void App::SetupProgramConfig() {
  // read config files and set up global vars
  const char* setup_file = "./src/setup.pconf";
  if (FileUtil::FileExists(setup_file) && FileUtil::MatchesExt(setup_file, ".pconf")) {
    auto setup_file_lines = FileUtil::ReadFileLines(setup_file);

    for (unsigned int li = 0; li < setup_file_lines.size(); li++) {
      auto line_tokens = StringUtil::SplitString(setup_file_lines[li], " ");

      if (line_tokens[0] == "." || line_tokens[0] == ".r") {
        if (config_data.count(line_tokens[1]) <= 0) {
          float vec_x = std::stof(line_tokens[2]);
          float vec_y = std::stof(line_tokens[3]);
          float vec_z = std::stof(line_tokens[4]);

          config_data[line_tokens[1]] = line_tokens[0] == ".r" ? Color::RGB((int)vec_x, (int)vec_y, (int)vec_z) : glm::vec3(vec_x, vec_y, vec_z);
        }
      }
    }
    
    std::cout << "STATUS-[SETUPCONF] Config file data set up." << std::endl;
  } else {
    std::cout << "ERROR-[SETUPCONF] Setup file does not match extensions or does not exist.";
  }
}

void App::SetupImGuiStyle() {
  // set general ui style settings - fonts, etc..
  imgui_io->Fonts->AddFontDefault();
  im_font_main = imgui_io->Fonts->AddFontFromFileTTF("./assets/fonts/Roboto-VariableFont.ttf", 28.0f);
  IM_ASSERT(im_font_main != NULL);

  // convert .pconf file global ui colors into ImGui compatible vectors
  ImVec4 cocad_ui_bg = ImVec4(config_data["glb_ui_bg"].x, config_data["glb_ui_bg"].y, config_data["glb_ui_bg"].z, 1.0f);
  ImVec4 cocad_ui_text = ImVec4(config_data["glb_ui_text"].x, config_data["glb_ui_text"].y, config_data["glb_ui_text"].z, 1.0f);
  ImVec4 cocad_ui_primary = ImVec4(config_data["glb_ui_primary"].x, config_data["glb_ui_primary"].y, config_data["glb_ui_primary"].z, 1.0f);
  ImVec4 cocad_ui_secondary = ImVec4(config_data["glb_ui_secondary"].x, config_data["glb_ui_secondary"].y, config_data["glb_ui_secondary"].z, 1.0f);
  ImVec4 cocad_ui_accent = ImVec4(config_data["glb_ui_accent"].x, config_data["glb_ui_accent"].y, config_data["glb_ui_accent"].z, 1.0f);

  // set ui colors
  imgui_style->Colors[ImGuiCol_WindowBg] = cocad_ui_bg;
  imgui_style->Colors[ImGuiCol_Text] = cocad_ui_text;
  
  imgui_style->Colors[ImGuiCol_TitleBg] = cocad_ui_secondary;
  imgui_style->Colors[ImGuiCol_TitleBgActive] = cocad_ui_secondary;
  
  imgui_style->Colors[ImGuiCol_TabHovered] = cocad_ui_secondary;
  imgui_style->Colors[ImGuiCol_Tab] = cocad_ui_bg;
  imgui_style->Colors[ImGuiCol_TabActive] = cocad_ui_secondary;

  imgui_style->Colors[ImGuiCol_FrameBg] = cocad_ui_secondary;
  imgui_style->Colors[ImGuiCol_FrameBgActive] = cocad_ui_secondary; 
  imgui_style->Colors[ImGuiCol_FrameBgHovered] = cocad_ui_secondary;
  imgui_style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  imgui_style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);

  imgui_style->Colors[ImGuiCol_Button] = cocad_ui_secondary;
  imgui_style->Colors[ImGuiCol_ButtonHovered] = cocad_ui_secondary;
  imgui_style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

}

void App::SetupGrid(bool regen) {
  std::vector<float> grid_vertices;
  std::vector<float> grid_axes_vertices;

  half_g_length = (grid_size * grid_unit_size) / 2.0f;

  // Grid vertices generation
  for (unsigned int r = 0; r < grid_size + 1; r++) {
    if (r == grid_size / 2) { continue; } 
    float x = r * grid_unit_size;
    float z1 = 0.0f;
    float z2 = grid_unit_size * grid_size;
    grid_vertices.insert(grid_vertices.end(), {x, 0.0f, z1});
    grid_vertices.insert(grid_vertices.end(), {1.0f, 1.0f, 1.0f});
    grid_vertices.insert(grid_vertices.end(), {x, 0.0f, z2});
    grid_vertices.insert(grid_vertices.end(), {1.0f, 1.0f, 1.0f});
  }

  for (unsigned int c = 0; c < grid_size + 1; c++) {
    if (c == grid_size / 2) { continue; } 
    float z = c * grid_unit_size;
    float x1 = 0.0f;
    float x2 = grid_unit_size * grid_size;

    grid_vertices.insert(grid_vertices.end(), {x1, 0.0f, z});
    grid_vertices.insert(grid_vertices.end(), {1.0f, 1.0f, 1.0f});
    grid_vertices.insert(grid_vertices.end(), {x2, 0.0f, z});
    grid_vertices.insert(grid_vertices.end(), {1.0f, 1.0f, 1.0f});
  }

  // Axis vertices generation - ensure only facing positive for testing rn!
  // X - AXIS
  grid_vertices.insert(grid_vertices.end(), {-half_g_length, 0.0f, 0.0f}); 
  grid_vertices.insert(grid_vertices.end(), {1.0f, 0.0f, 0.0f}); 
  grid_vertices.insert(grid_vertices.end(), {half_g_length, 0.0f, 0.0f}); 
  grid_vertices.insert(grid_vertices.end(), {0.8f, 0.0f, 0.0f}); 

  // Y - AXIS
  grid_vertices.insert(grid_vertices.end(), {0.0f, 0.0f, 0.0f}); 
  grid_vertices.insert(grid_vertices.end(), {0.0f, 1.0f, 0.0f}); 
  grid_vertices.insert(grid_vertices.end(), {0.0f, 8.0f, 0.0f}); 
  grid_vertices.insert(grid_vertices.end(), {0.0f, 0.8f, 0.0f}); 

  // Z - AXIS
  glm::vec3 z_blue = Color::RGB(14, 107, 237);
  grid_vertices.insert(grid_vertices.end(), {0.0f, 0.0f, -half_g_length}); 
  grid_vertices.insert(grid_vertices.end(), {z_blue.x, z_blue.y, z_blue.z}); 
  grid_vertices.insert(grid_vertices.end(), {0.0f, 0.0f, half_g_length}); 
  grid_vertices.insert(grid_vertices.end(), {z_blue.x, z_blue.y, z_blue.z}); 
  
  if (regen != true) {
    glGenVertexArrays(1, &this->grid_VAO);
    glGenBuffers(1, &this->grid_VBO);
  }

  glBindVertexArray(this->grid_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, this->grid_VBO);
  glBufferData(GL_ARRAY_BUFFER, grid_vertices.size() * sizeof(float), grid_vertices.data(), GL_STATIC_DRAW);
  this->grid_vertex_count = grid_vertices.size() / 6;

  if (regen != true) {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }
  
  grid_unit_size_prev = grid_unit_size;
  grid_size_prev = grid_size;
}

void App::RenderGrid() {
  ResourceManager::GetShader("baser").Use();
  ResourceManager::GetShader("baser").setUMat4("m4_view", glb_view_matrix);
  ResourceManager::GetShader("baser").setUFloat("alpha", 0.2f);
  glLineWidth(2.0f);
  glBindVertexArray(this->grid_VAO);
  this->grid_model_mat = glm::translate(this->grid_model_mat, glm::vec3(-half_g_length, 0.0f, -half_g_length));
  ResourceManager::GetShader("baser").setUMat4("m4_model", grid_model_mat);
  glDrawArrays(GL_LINES, 0, grid_vertex_count - 6);
  ResourceManager::GetShader("baser").setUFloat("alpha", 0.60f);
  this->grid_model_mat = glm::translate(this->grid_model_mat, glm::vec3(half_g_length, 0.0f, half_g_length));
  ResourceManager::GetShader("baser").setUMat4("m4_model", grid_model_mat);
  glLineWidth(4.0f);
  glDrawArrays(GL_LINES, grid_vertex_count - 6, 6);
}

void App::AdjustScalingFromDPI() {
  // NOTE: we are assuming uniform scaling
  float scale_x, scale_y;
  glfwGetWindowContentScale(this->window, &scale_x, &scale_y);
  std::cout << "STATUS-[SCREEN_SCALE_VALUE] " << scale_x << "\n";
  imgui_style->ScaleAllSizes(scale_x); 
}

void App::InitWindow() {
  this->SetupProgramConfig();

  // ============= GLOBAL SETUP ============== //
  ResourceManager::GenShader("./src/shaders/def3D.vs", "./src/shaders/def3D.fs", "def3d");
  glb_shader_3D = ResourceManager::GetShader("def3d");
  glb_shader_3D.Use();
  glb_persp_proj = glm::perspective(glb_fov, (float)fb_width / (float)fb_height, glb_NEAR, glb_FAR);
  glb_view_matrix = glm::mat4(1.0f);
  glb_shader_3D.setUMat4("m4_view", glb_view_matrix);
  glb_shader_3D.setUMat4("m4_proj", glb_persp_proj);
  glb_shader_3D.setUVec3("v3_light_origin", config_data["glb_light_loc"]);
  
  i_mdl_color = config_data["glb_mdl_color"];
  glb_shader_3D.setUVec3("v3_model_color", i_mdl_color);
  glb_shader_3D.setUFloat("diffuse_intensity", this->light_intensity);
  camera.SetZoomDistance(-25.0f);
  camera.SetDefaultRotPosition(45.0f, 225.0f);
  glb_view_matrix = camera.UpdateSphericalCameraClassic(0.0f, 0.0f);

  ResourceManager::GenShader("./src/shaders/baser.vs", "./src/shaders/baser.fs", "baser");
  Shader baser = ResourceManager::GetShader("baser");
  baser.Use();
  baser.setUMat4("m4_view", glb_view_matrix);
  baser.setUMat4("m4_proj", glb_persp_proj);
  baser.setUMat4("m4_model", grid_model_mat);

  IMGUI_CHECKVERSION(); 
  ImGui::CreateContext();
  ImGui::SetCurrentContext(ImGui::GetCurrentContext());
  imgui_io = &ImGui::GetIO(); 
  (void)imgui_io;
  imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(this->window, true); 
  ImGui_ImplOpenGL3_Init("#version 460");
  imgui_style = &ImGui::GetStyle();
  this->SetupImGuiStyle();
  this->AdjustScalingFromDPI();
 
  // TEMP SOLUTION TO WINDOW SIZING AND LOCATION
  window_sizes["win_obj_prop"] = glm::vec2(500.0f, fb_height - 700.0f);
  window_locs["win_obj_prop"] = glm::vec2(0.0f, 0.0f);
  
  window_sizes["win_settings"] = glm::vec2(500.0f, 700.0f);
  window_locs["win_settings"] = glm::vec2(0.0f, fb_height - 700.0f);

  window_sizes["win_sesh"] = glm::vec2(500.0f, 400.0f);
  window_locs["win_sesh"] = glm::vec2(fb_width - window_sizes["win_sesh"].x, 0.0f);
  
  window_sizes["win_chat"] = glm::vec2(500.0f, fb_height - 400.0f);
  window_locs["win_chat"] = glm::vec2(fb_width - window_sizes["win_chat"].x, fb_height - window_sizes["win_chat"].y);

  // Loading and Setting Up Model 
  kube = OBJLoader::LoadModel("./assets/models/monke.obj"); 
  
  Editor::GenerateRepr(kube);
  Editor::SetUp(&this->camera, this->fb_width, this->fb_height);
  Editor::repr.sel_color = config_data["edit_vert_sel"];
  Editor::repr.desel_color = config_data["edit_vert_desel"];

  ResourceManager::GenShader("./src/shaders/baser_i.vs", "./src/shaders/baser_i.fs", "baser_i");
  glb_shader_vert = ResourceManager::GetShader("baser_i");
  glb_shader_vert.Use();
  glb_shader_vert.setUMat4("m4_view", glb_view_matrix);
  glb_shader_vert.setUMat4("m4_proj", glb_persp_proj);
  glb_shader_vert.setUFloat("alpha", 1.0f);

  bg_col = config_data["glb_bg"];
  this->SetupGrid();
  this->LimitFPS(true, 60);
}


// ============================================================= //
//                  --- INPUT SECTION ---
// ============================================================= //
void App::MouseButtonInput(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_MIDDLE) { 
    if (action == GLFW_PRESS) {
      glfwGetCursorPos(this->window, &this->mouse_x, &this->mouse_y);

      if (isDragging == false) { 
        drag_loc_start_x = mouse_x; 
        drag_loc_start_y = mouse_y; 
        isDragging = true;
      }

    } else if (action == GLFW_RELEASE) {
      isDragging = false;
    }
  }


  if (action == GLFW_PRESS) {
    keymap[button] = true;
    glfwGetCursorPos(this->window, &this->mouse_x, &this->mouse_y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && Editor::GetSelectionMode() != COCAD_SELMODE_NONE) {
      Editor::CastRay(this->mouse_x, this->mouse_y, this->glb_view_matrix, this->glb_persp_proj);
    }

  } else { 
    keymap[button] = false;
  }

}

void App::MouseScrollInput(double xoffset, double yoffset) {
  float n_zoom = camera.GetZoomDistance() + (0.5f * (float)yoffset);
  camera.SetZoomDistance(n_zoom);
  glb_view_matrix = camera.UpdateSphericalCameraClassic(0.0f, 0.0f);

  float mvd_new_scale = (float)yoffset * 0.00025f;
  Editor::UpdateVertOverlayInstanceScale(-mvd_new_scale);
}


void App::KeyboardInput(int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) { keymap[key] = true; }
  else if (action == GLFW_RELEASE) { keymap[key] = false; }
}


// ============================================================= //
//                    --- UPDATE LOOP ---
// ============================================================= //
void App::Update() {
  
  // Updating Camera
  glfwGetCursorPos(this->window, &drag_loc_end_x, &drag_loc_end_y);
  double dt_x = (drag_loc_end_x - drag_loc_start_x) / camera.sensitivity;  
  double dt_y = (drag_loc_end_y - drag_loc_start_y) / camera.sensitivity;  

  if (keymap[GLFW_MOUSE_BUTTON_MIDDLE]) {
    glb_view_matrix = camera.UpdateSphericalCameraClassic(-dt_x, dt_y);
    camera.isUpdated = true;
  } else {
    if (camera.isUpdated) {
      float new_theta = camera.rotational_pos.x + glm::radians(-dt_x);
      float new_phi = camera.rotational_pos.y + glm::radians(dt_y);
      camera.rotational_pos.x = new_theta; 
      camera.rotational_pos.y = MathUtil::Clamp(new_phi, glm::radians(camera.min_phi), glm::radians(camera.max_phi));
      camera.isUpdated = false;
    }
  }

  // Updating Light Position
  glm::vec3 new_light_loc = glm::normalize(glm::vec3(camera.pos.x, camera.pos.y, camera.pos.z));
  glb_shader_3D.Use();
  glb_shader_3D.setUVec3("v3_light_origin", new_light_loc);


  // KEYBOARD STUFF FINALLY!
  if (keymap[GLFW_KEY_0]) { Editor::SetSelectionMode(COCAD_SELMODE_NONE); }
  else if (keymap[GLFW_KEY_1]) { Editor::SetSelectionMode(COCAD_SELMODE_VERT); }
  else if (keymap[GLFW_KEY_2]) { Editor::SetSelectionMode(COCAD_SELMODE_EDGE); }
  else if (keymap[GLFW_KEY_3]) { Editor::SetSelectionMode(COCAD_SELMODE_FACE); }

}

// ============================================================= //
//                    --- RENDER LOOP ---
// ============================================================= //
void App::Render() {
  glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glClearColor(bg_col.r, bg_col.g, bg_col.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->RenderGrid();

  glb_shader_3D.Use();
  glb_shader_3D.setUMat4("m4_view", glb_view_matrix);
  glb_shader_3D.setUFloat("diffuse_intensity", light_intensity);
  glb_shader_3D.setUMat4("m4_model", kube.mdl_matrix);
  //kube.mdl_matrix = glm::scale(kube.mdl_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
  kube.RenderModel(glb_shader_3D);

  if (Editor::GetSelectionMode() == COCAD_SELMODE_VERT) {
    glb_shader_vert.Use();
    glb_shader_vert.setUMat4("m4_view", glb_view_matrix);
    Editor::RenderVertOverlay(glb_shader_vert);   
  }

  if (grid_size != grid_size_prev || grid_unit_size != grid_unit_size_prev) {
    this->SetupGrid(true);
  }

  // ==================== UI ========================= //
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::PushFont(im_font_main);

  //ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  //ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  
  ImGui::SetNextWindowPos(ImVec2(window_locs["win_obj_prop"].x, window_locs["win_obj_prop"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_obj_prop"].x, window_sizes["win_obj_prop"].y));

  CoCadUI::WindowStart("Model Properties");
  
  CoCadUI::WindowEnd();

  ImGui::SetNextWindowPos(ImVec2(window_locs["win_settings"].x, window_locs["win_settings"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_settings"].x, window_locs["win_settings"].y));

  CoCadUI::WindowStart("Settings");
  ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);
  ImGui::BeginTabBar("#left_tabs_bar");
  if (ImGui::BeginTabItem("General")) {
    
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
    ImGui::SliderFloat(" Camera Sensitivity", &camera.sensitivity, 0.5f, 25.0f, "%.1f");
    ImGui::SliderFloat(" Light Intensity", &light_intensity, 0.0f, 1.0f, "%.2f");
    ImGui::PopItemWidth();
  
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);
    ImGui::InputFloat(" Grid Unit Size", &grid_unit_size, 0.1f, 1.0f, "%.2f");
    grid_unit_size = MathUtil::Clamp(grid_unit_size, 0.0f, 500.0f);
    ImGui::InputInt(" Grid Size", &grid_size);
    grid_size = (int)MathUtil::Clamp((float)grid_size, 0.0f, 5000.0f);
    ImGui::PopItemWidth();
    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem("Colors")) {
    
    /* @TODO: Fix this - doesnt match color + crashes applications especially when in edit mode for some reason
     * note: problem stems from config_data i believe, removing intermediate variables would probably solve the problem
    */ // 02:00 Lukas Note - 17/04/2025
    CoCadUI::PopColorPicker("Background Color", CoCadUI::GlmVec3ToImVec4(config_data["glb_bg"]));
    ImVec4 bg_col_vec4 = CoCadUI::col_pop_pickers["Background Color"].color;
    bg_col = glm::vec3(bg_col_vec4.x, bg_col_vec4.y, bg_col_vec4.z);

    CoCadUI::PopColorPicker("Model Color", CoCadUI::GlmVec3ToImVec4(config_data["glb_mdl_color"]));
    ImVec4 mdl_color = CoCadUI::col_pop_pickers["Model Color"].color;
    glb_shader_3D.Use();
    glb_shader_3D.setUVec3("v3_model_color", mdl_color);

    CoCadUI::PopColorPicker("Vertex Color", CoCadUI::GlmVec3ToImVec4(config_data["edit_vert_desel"]));
    ImVec4 vert_col = CoCadUI::col_pop_pickers["Vertex Color"].color;
    Editor::repr.desel_color = glm::vec3(vert_col.x, vert_col.y, vert_col.z);

    CoCadUI::PopColorPicker("Vertex Selected Color", ImVec4(config_data["edit_vert_sel"].x, config_data["edit_vert_sel"].y, config_data["edit_vert_sel"].z, 1.0f));
    ImVec4 vert_sel_col = CoCadUI::col_pop_pickers["Vertex Selected Color"].color;
    Editor::repr.sel_color = glm::vec3(vert_sel_col.x, vert_sel_col.y, vert_sel_col.z);
    /**/

    ImGui::EndTabItem();
  }

  ImGui::EndTabBar();
  ImGui::PopStyleVar();
  CoCadUI::WindowEnd();

  ImGui::SetNextWindowPos(ImVec2(window_locs["win_chat"].x, window_locs["win_chat"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_chat"].x, window_sizes["win_chat"].y));

  CoCadUI::WindowStart("Chat");
  ImGui::Text("[User 1]: Hello, how is the project going?");
  ImGui::Text("[User 2]: Hi! its going okay :)");
  CoCadUI::WindowEnd();
 
  ImGui::SetNextWindowPos(ImVec2(window_locs["win_sesh"].x, window_locs["win_sesh"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_sesh"].x, window_sizes["win_sesh"].y));

  CoCadUI::WindowStart("Session Settings");
  if (ImGui::Button("Host Session")) {
    std::cout << "[DEBUG] Hosting session...";
  }
  
  if (ImGui::Button("End Session")) {
    std::cout << "[DEBUG] Ending Session..";
  }
  ImGui::Text("Session ID: 027xf2");
  CoCadUI::WindowEnd();

  ImGui::PopFont();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::CleanWindow() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
