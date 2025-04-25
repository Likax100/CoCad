#include "App.h"

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>

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

          //config_data[line_tokens[1]] = (line_tokens[0] == ".r") ? CoCad::Color::RGB((int)vec_x, (int)vec_y, (int)vec_z) : glm::vec3(vec_x, vec_y, vec_z);
          config_data[line_tokens[1]] = (line_tokens[0] == ".r") ? glm::vec3(vec_x/255.0f, vec_y/255.0f, vec_z/255.0f) : glm::vec3(vec_x, vec_y, vec_z);
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

  ImFontConfig config;
  config.MergeMode = true;
  config.GlyphMinAdvanceX = 13.0f;
  static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
  im_font_icon = imgui_io->Fonts->AddFontFromFileTTF("./assets/fonts/Font Awesome 6 Solid-900.otf", 28.0f, &config, icon_ranges);
  IM_ASSERT(im_font_icon != NULL);

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
  //glm::vec3 z_blue = CoCad::Color::RGB(14, 107, 237);
  glm::vec3 z_blue = glm::vec3(14.0f/255.0f, 107.0f/255.0f, 237.0f/255.0f);
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

  // ============= NETWORK SETUP ============= //
  cc_client.connect("127.0.0.1", 60000);

  // ============= GLOBAL SETUP ============== //
  ResourceManager::GenShader("./src/shaders/def3D.vs", "./src/shaders/def3D.fs", "def3d");
  glb_shader_3D = ResourceManager::GetShader("def3d");
  glb_shader_3D.Use();
  glb_persp_proj = glm::perspective(glb_fov, (float)fb_width / (float)fb_height, glb_NEAR, glb_FAR);
  glb_view_matrix = glm::mat4(1.0f);
  glb_shader_3D.setUMat4("m4_view", glb_view_matrix);
  glb_shader_3D.setUMat4("m4_proj", glb_persp_proj);
  glb_shader_3D.setUVec3("v3_light_origin", config_data["glb_light_loc"]);
  
  //glb_shader_3D.setUVec3("v3_model_color", config_data["glb_mdl_color"]);
  glb_shader_3D.setUFloat("diffuse_intensity", this->light_intensity);
  camera.SetZoomDistance(-25.0f);
  camera.SetDefaultRotPosition(45.0f, 225.0f);
  glb_view_matrix = camera.UpdateSphericalCameraClassic(0.0f, 0.0f);

  ResourceManager::GenShader("./src/shaders/baser.vs", "./src/shaders/baser.fs", "baser");
  glb_shader_baser = ResourceManager::GetShader("baser");
  glb_shader_baser.Use();
  glb_shader_baser.setUMat4("m4_view", glb_view_matrix);
  glb_shader_baser.setUMat4("m4_proj", glb_persp_proj);
  glb_shader_baser.setUMat4("m4_model", grid_model_mat);

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

  // Login Window
  window_sizes["win_login"] = glm::vec2(fb_width, fb_height);
  window_locs["win_login"] = glm::vec2(0.0f, 0.0f);

  // TEMP SOLUTION TO WINDOW SIZING AND LOCATION
  window_sizes["win_obj_prop"] = glm::vec2(500.0f, fb_height - 900.0f);
  window_locs["win_obj_prop"] = glm::vec2(0.0f, 0.0f);
 
  window_sizes["win_tools"] = glm::vec2(500.0f, 400.0f);
  window_locs["win_tools"] = glm::vec2(0.0f, fb_height - 900.0f);

  window_sizes["win_settings"] = glm::vec2(500.0f, 500.0f);
  window_locs["win_settings"] = glm::vec2(0.0f, fb_height - 500.0f);

  window_sizes["win_sesh"] = glm::vec2(500.0f, 400.0f);
  window_locs["win_sesh"] = glm::vec2(fb_width - window_sizes["win_sesh"].x, 0.0f);
  
  window_sizes["win_chat"] = glm::vec2(500.0f, fb_height - 400.0f);
  window_locs["win_chat"] = glm::vec2(fb_width - window_sizes["win_chat"].x, fb_height - window_sizes["win_chat"].y);

  // Loading and Setting Up Model
  // load default cube if no model selected
  mdl = OBJLoader::LoadModel("./assets/models/cube.obj"); 
  
  Editor::r_repr.mdl_color = config_data["glb_mdl_color"];
  Editor::repr.sel_color = config_data["edit_vert_sel"];
  Editor::repr.desel_color = config_data["edit_vert_desel"];
  Editor::GenerateRepr(mdl);
  Editor::SetUp(&this->camera, this->fb_width, this->fb_height);

  ResourceManager::GenShader("./src/shaders/baser_i.vs", "./src/shaders/baser_i.fs", "baser_i");
  glb_shader_vert = ResourceManager::GetShader("baser_i");
  glb_shader_vert.Use();
  glb_shader_vert.setUMat4("m4_view", glb_view_matrix);
  glb_shader_vert.setUMat4("m4_proj", glb_persp_proj);
  glb_shader_vert.setUFloat("alpha", 1.0f);

  bg_col = CoCadUI::GlmVec3ToImVec4(config_data["glb_bg"]);
  i_mdl_color = CoCadUI::GlmVec3ToImVec4(config_data["glb_mdl_color"]);
  i_dvert_color = CoCadUI::GlmVec3ToImVec4(config_data["edit_vert_desel"]);
  i_svert_color = CoCadUI::GlmVec3ToImVec4(config_data["edit_vert_sel"]);
  accent_col = CoCadUI::GlmVec3ToImVec4(config_data["glb_ui_accent"]);

  this->SetupGrid();
  this->LimitFPS(true, 60);
}


// ============================================================= //
//                  --- INPUT SECTION ---
// ============================================================= //
void App::MouseButtonInput(int button, int action, int mods) {
  if (imgui_io->WantCaptureMouse) { return; } // disable mouse actions when interacting with imgui windows  


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


  if (action == GLFW_PRESS && mouse_click_handled == false) {
    keymap[button] = true;
    glfwGetCursorPos(this->window, &this->mouse_x, &this->mouse_y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && Editor::GetSelectionMode() != COCAD_SELMODE_NONE) {
      Editor::CastRay(this->mouse_x, this->mouse_y, this->glb_view_matrix, this->glb_persp_proj);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && Editor::move_mode_active) {
      Editor::move_mode_active = false;
      Editor::move_dat_update = true;
    }

    mouse_click_handled = true;
  } else { 
    keymap[button] = false;
    mouse_click_handled = false;
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
  if (imgui_io->WantCaptureKeyboard) { 
    return; //disables glfw key processing if in imgui window :) 
  }
  
  active_keymod = mods;

  if (action == GLFW_PRESS) { keymap[key] = true;}
  else if (action == GLFW_RELEASE) { keymap[key] = false;}

  // handle single press processed keybinds
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_M) {
      if (Editor::GetSelectionMode() != COCAD_SELMODE_NONE) { Editor::move_mode_active = !Editor::move_mode_active; }
      glfwGetCursorPos(this->window, &start_loc_x, &start_loc_y);
    }

    if (key == GLFW_KEY_P) { 
      std::cout << "[SELF] Attempting to ping server...\n";
      cc_client.PingServer(); 
    }

    if (key == GLFW_KEY_X) { 
      axis_locked[0] = true; axis_locked[1] = false; axis_locked[2] = false; }

    else if (key == GLFW_KEY_Y) { 
      axis_locked[0] = false; axis_locked[1] = true; axis_locked[2] = false; }

    else if (key == GLFW_KEY_Z) { 
      axis_locked[0] = false; axis_locked[1] = false; axis_locked[2] = true; }
  }

}


// ============================================================= //
//                    --- UPDATE LOOP ---
// ============================================================= //
// @proc
void App::ProcessClient() {
  if (cc_client.is_connected()) {
    if (!cc_client.get_incoming_queue().empty()) {
      auto m = cc_client.get_incoming_queue().pop_front().m;
      auto packet_data = StringUtil::SplitString(m.dat, " ");

      switch (m.head.ID) {
        case MessageTypes::ServerAccept:
          std::cout << "Server has accepted the connection!\n";
          break;

        case MessageTypes::ServerPing:
          std::cout << "Pinging Bounced back and successful :)\n";
          break;

        case MessageTypes::ccAuthenticationOutcome: {
          if (packet_data[0] == "1") {
            logged_in = true;
            user.username = login_username;
            user.pass = login_password;
            user.client_ID = std::stoi(packet_data[1]);
            user.is_host = false;

            sesh_info.currently_in_session = false;
            sesh_info.session_host = -1;
          }
          std::string outc = packet_data[0] == "1" ? "Success" : "Failure";
          std::cout << "[SERVER-RESPONSE] Authentication Outcome " << outc << "\n";
        } break;

        case MessageTypes::ccStatusHostExists: {
          if (packet_data[0] == "1") {
            sesh_info.host_exists = true;
            sesh_info.session_host = std::stoi(packet_data[1]);
          } else {
            sesh_info.host_exists = false;
            sesh_info.session_host = -1;
          }
        } break;
      
        case MessageTypes::ccSessionHostRequestOutcome: {
          if (packet_data[0] == "1") { user.is_host = true; }
          else { user.is_host = false; }
        } break;

        case MessageTypes::ccRequestJoinSessionOutcome: {
          if (packet_data[0] == "1") { sesh_info.currently_in_session = true; }
        } break;

        case MessageTypes::ccOpRequestSHModelData: {
          if (std::stoi(packet_data[0]) == user.client_ID) {
            // generate and send repr data to all clients - safe
            CoCadNet::msg<MessageTypes> repr_dat;
            repr_dat.head.ID = MessageTypes::ccOpSHSentModelData;
             
            repr_dat.dat = "ogvc";
            for (int ogvc_i = 0; ogvc_i < Editor::repr.og_vert_cpy.size(); ogvc_i++) { 
              repr_dat.dat += " " + std::to_string(Editor::repr.og_vert_cpy[ogvc_i]); 
            }
            
            repr_dat.dat += "\nuv";
            for (int uv_i = 0; uv_i < Editor::repr.unique_verts.size(); uv_i++) { 
              repr_dat.dat += " " + std::to_string(Editor::repr.unique_verts[uv_i]); 
            }
            
            repr_dat.dat += "\nufn";
            for (int ufn_i = 0; ufn_i < Editor::repr.unique_face_normals.size(); ufn_i++) { 
              repr_dat.dat += " " + std::to_string(Editor::repr.unique_face_normals[ufn_i]); 
            }
            
            repr_dat.dat += "\nfi";
            for (int fi_i = 0; fi_i < Editor::repr.face_indices.size(); fi_i++) { 
              repr_dat.dat += " " + std::to_string(Editor::repr.face_indices[fi_i]); 
            }
            
            repr_dat.dat += "\nnr";
            for (int nr_i = 0; nr_i < Editor::repr.normals_redundant.size(); nr_i++) { 
              repr_dat.dat += " " + std::to_string(Editor::repr.normals_redundant[nr_i]); 
            }
            
            cc_client.send_msg(repr_dat);
          }
        } break;

        case MessageTypes::ccOpSHSentModelData: {
          Editor::ClearRepr();
          auto repr_data = StringUtil::SplitString(m.dat, "\n");

          for (auto line: repr_data) {
            auto l_data = StringUtil::SplitString(line, " ");

            if (l_data[0] == "ogvc") {
              for (unsigned int el = 1; el < l_data.size(); el++) { Editor::repr.og_vert_cpy.push_back(std::stof(l_data[el])); }
            } else if (l_data[0] == "uv") {
              for (unsigned int el2 = 1; el2 < l_data.size(); el2++) { Editor::repr.unique_verts.push_back(std::stof(l_data[el2])); }
            } else if (l_data[0] == "ufn") {
              for (unsigned int el3 = 1; el3 < l_data.size(); el3++) { Editor::repr.unique_face_normals.push_back(std::stof(l_data[el3])); }
            } else if (l_data[0] == "fi") {
              for (unsigned int el4 = 1; el4 < l_data.size(); el4++) { Editor::repr.face_indices.push_back(static_cast<unsigned int>(std::stoul(l_data[el4])) ); }
            } else if (l_data[0] == "nr") {
              for (unsigned int el5 = 1; el5 < l_data.size(); el5++) { Editor::repr.normals_redundant.push_back(static_cast<unsigned int>(std::stoul(l_data[el5])) ); }
            }
          }
          
          Editor::instance_data_updated = true;
          Editor::instance_color_updated = true;
          Editor::edge_data_updated = true;

          // call recalculation editor funcs
        } break;
      }

    }
  }
}

void App::Update() {
  ProcessClient();

  // -- set global editor selected verts set
  Editor::sel_unique_verts.clear();
  Editor::sel_unique_verts = std::set(Editor::repr.selected_verts.begin(), Editor::repr.selected_verts.end());

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

  if (active_keymod == GLFW_MOD_CONTROL) {
    Editor::PassModKeyControl(true); 
  } else { Editor::PassModKeyControl(false); }

  // KEYBOARD INPUT MANAGEMENT 
  if (keymap[GLFW_KEY_0]) { Editor::SetSelectionMode(COCAD_SELMODE_NONE); }
  else if (keymap[GLFW_KEY_1]) { Editor::SetSelectionMode(COCAD_SELMODE_VERT); }
  else if (keymap[GLFW_KEY_2]) { Editor::SetSelectionMode(COCAD_SELMODE_EDGE); }
  else if (keymap[GLFW_KEY_3]) { Editor::SetSelectionMode(COCAD_SELMODE_FACE); }

  // PROCESS OPERATIONS
  // -- moving selected verts
  if (Editor::move_mode_active) {

    float move_by_y = (drag_loc_end_y - start_loc_y) / 120.0f;
    float move_by_x = (drag_loc_end_x - start_loc_x) / 120.0f;
    
    for (auto v_sel: Editor::sel_unique_verts) {
      
      /*
      // free move mode calcs
      float vx = Editor::repr.og_vert_cpy[(v_sel*3)];
      float vy = Editor::repr.og_vert_cpy[(v_sel*3)+1];
      float vz = Editor::repr.og_vert_cpy[(v_sel*3)+2];
      glm::vec3 p = glm::vec3(vx, vy, vz);

      glm::vec3 plane_normal = glm::normalize(camera.pos - p);
      
      // -- selecting random vector to find perpendicular component of plane normal
      glm::vec3 random_vec;
      if (abs(plane_normal.y) < 0.99f) { random_vec = glm::vec3(0.0f, 1.0f, 0.0f); }
      else { random_vec = glm::vec3(1.0f, 0.0f, 0.0f); }
      
      glm::vec3 plane_tangent_x = glm::normalize(glm::cross(plane_normal, random_vec));
      glm::vec3 plane_tangent_y = glm::normalize(glm::cross(plane_normal, plane_tangent_y)); //calc second so moving point is easier

      glm::vec3 offset = (move_by_x * plane_tangent_x) + (move_by_y * plane_tangent_y);
      offset.y = -offset.y;

      Editor::repr.unique_verts[(v_sel*3)] = Editor::repr.og_vert_cpy[(v_sel*3)] + offset.x;
      Editor::repr.unique_verts[(v_sel*3)+1] = Editor::repr.og_vert_cpy[(v_sel*3)+1] + offset.y;
      Editor::repr.unique_verts[(v_sel*3)+2] = Editor::repr.og_vert_cpy[(v_sel*3)+2] + offset.z;

      Editor::mvd_c_positions[v_sel] = Editor::og_mvdc_pos_cpy[v_sel] + offset;*/
   
      // locked movement mode
      if (axis_locked[0]) {
        Editor::repr.unique_verts[(v_sel*3)] = Editor::repr.og_vert_cpy[(v_sel*3)] + move_by_x;
        Editor::mvd_c_positions[v_sel].x = Editor::og_mvdc_pos_cpy[v_sel].x + move_by_x;
      } else if (axis_locked[1]) {
        Editor::repr.unique_verts[(v_sel*3)+1] = Editor::repr.og_vert_cpy[(v_sel*3)+1] - move_by_y;
        Editor::mvd_c_positions[v_sel].y = Editor::og_mvdc_pos_cpy[v_sel].y - move_by_y;
      } else if (axis_locked[2]) {
        Editor::repr.unique_verts[(v_sel*3)+2] = Editor::repr.og_vert_cpy[(v_sel*3)+2] + move_by_y;
        Editor::mvd_c_positions[v_sel].z = Editor::og_mvdc_pos_cpy[v_sel].z + move_by_y;
      }
    }

    Editor::edge_data_updated = true;
    Editor::instance_data_updated = true;
  }

  if (Editor::move_dat_update) {
    Editor::repr.og_vert_cpy = Editor::repr.unique_verts;
    Editor::og_mvdc_pos_cpy = Editor::mvd_c_positions;
    Editor::move_dat_update = false;
  }

}

// ============================================================= //
//                    --- RENDER LOOP ---
// ============================================================= //
void App::Render() {
  glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glClearColor(bg_col.x, bg_col.y, bg_col.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->RenderGrid();

  glb_shader_3D.Use();
  glb_shader_3D.setUMat4("m4_view", glb_view_matrix);
  glb_shader_3D.setUFloat("diffuse_intensity", light_intensity);
  glb_shader_3D.setUMat4("m4_model", mdl.mdl_matrix);
  Editor::r_repr.mdl_color = CoCadUI::ImVec4ToGlmVec3(i_mdl_color);
  Editor::SetSelectionColors(CoCadUI::ImVec4ToGlmVec3(i_svert_color), CoCadUI::ImVec4ToGlmVec3(i_dvert_color));
  Editor::SetMVDScalings(mvd_size, mvd_sel_radius_fact);

  Editor::Render(glb_shader_3D);

  glb_shader_vert.Use();
  glb_shader_vert.setUMat4("m4_view", glb_view_matrix);

  glb_shader_baser.Use();
  glb_shader_baser.setUMat4("m4_view", glb_view_matrix);
  
  unsigned int prev_selection_mode = Editor::GetSelectionMode();
  
  if (prev_selection_mode == COCAD_SELMODE_VERT) {
    Editor::RenderVertOverlay(glb_shader_vert);   
  } 
  if (prev_selection_mode == COCAD_SELMODE_EDGE) {
    Editor::RenderEdgeOverlay(glb_shader_baser);
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
  bool entries_modified = false;

  ImGui::Text("Model Origin:");
  entries_modified |= ImGui::InputFloat3("##Model Origin", Editor::mdl_glb_position, "%.2f");
  ImGui::Text("Model Scale:");
  entries_modified |= ImGui::InputFloat3("##Model Scale", Editor::mdl_glb_scale, "%.2f");
  ImGui::Text("Model Rotation:");
  entries_modified |= ImGui::InputInt3("##Model Rotation", Editor::mdl_glb_rot);

  if (entries_modified) { Editor::mdl_glb_updated = true; }

  CoCadUI::WindowEnd();

  ImGui::SetNextWindowPos(ImVec2(window_locs["win_tools"].x, window_locs["win_tools"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_tools"].x, window_sizes["win_tools"].y));

  CoCadUI::WindowStart("Tools");
  ImGui::Text("Selection Modes:");
  
  if (prev_selection_mode == COCAD_SELMODE_NONE) { ImGui::PushStyleColor(ImGuiCol_Text, accent_col); }
  if (ImGui::Button(ICON_FA_EYE " None")) { Editor::SetSelectionMode(COCAD_SELMODE_NONE); }
  if (prev_selection_mode == COCAD_SELMODE_NONE) { ImGui::PopStyleColor(); }
  ImGui::SameLine();

  if (prev_selection_mode == COCAD_SELMODE_FACE) { ImGui::PushStyleColor(ImGuiCol_Text, accent_col); }
  if (ImGui::Button(ICON_FA_DICE_D6 " Face")) { Editor::SetSelectionMode(COCAD_SELMODE_FACE); }
  if (prev_selection_mode == COCAD_SELMODE_FACE) { ImGui::PopStyleColor(); }
  ImGui::SameLine();

  if (prev_selection_mode == COCAD_SELMODE_EDGE) { ImGui::PushStyleColor(ImGuiCol_Text, accent_col); }
  if (ImGui::Button(ICON_FA_CIRCLE_NODES " Edge")) { Editor::SetSelectionMode(COCAD_SELMODE_EDGE); }
  if (prev_selection_mode == COCAD_SELMODE_EDGE) { ImGui::PopStyleColor(); }
  ImGui::SameLine();

  if (prev_selection_mode == COCAD_SELMODE_VERT) { ImGui::PushStyleColor(ImGuiCol_Text, accent_col); }
  if (ImGui::Button(ICON_FA_XMARK " Vertex")) { Editor::SetSelectionMode(COCAD_SELMODE_VERT); }
  if (prev_selection_mode == COCAD_SELMODE_VERT) { ImGui::PopStyleColor(); }

  ImGui::Text("Mesh Operations:");
  bool prev_move_mode = Editor::move_mode_active;
  
  if (ImGui::Button(ICON_FA_ROTATE_LEFT " Reset Mesh")) { 
    Editor::GenerateRepr(mdl);
    Editor::RecalculateMVD();
  }
  //ImGui::SameLine();

  if (prev_move_mode) { ImGui::PushStyleColor(ImGuiCol_Text, accent_col); }
  if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT " Move Tool")) { 
    if (Editor::GetSelectionMode() != COCAD_SELMODE_NONE) { Editor::move_mode_active = !Editor::move_mode_active; }
  }
  if (prev_move_mode) { ImGui::PopStyleColor(); }
  ImGui::SameLine();

  bool prev_axis_locked[3];
  prev_axis_locked[0] = axis_locked[0];
  prev_axis_locked[1] = axis_locked[1];
  prev_axis_locked[2] = axis_locked[2];

  if(prev_axis_locked[0] == true) { ImGui::PushStyleColor(ImGuiCol_Text, accent_col); }
  if (ImGui::Button(ICON_FA_LOCK " X")) {
    axis_locked[0] = true;
    axis_locked[1] = false;
    axis_locked[2] = false;
  }
  if(prev_axis_locked[0] == true) { ImGui::PopStyleColor(); }
  ImGui::SameLine();

  if(prev_axis_locked[1] == true) { ImGui::PushStyleColor(ImGuiCol_Text, accent_col); }
  if (ImGui::Button(ICON_FA_LOCK " Y")) {
    axis_locked[0] = false;
    axis_locked[1] = true;
    axis_locked[2] = false;
  }
  if(prev_axis_locked[1] == true) { ImGui::PopStyleColor(); }
  ImGui::SameLine();

  if(prev_axis_locked[2] == true) { ImGui::PushStyleColor(ImGuiCol_Text, accent_col); }
  if (ImGui::Button(ICON_FA_LOCK " Z")) {
    axis_locked[0] = false;
    axis_locked[1] = false;
    axis_locked[2] = true;
  }
  if(prev_axis_locked[2] == true) { ImGui::PopStyleColor(); }
  
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
    ImGui::SliderFloat(" Vertex Display Size", &mvd_size, 0.001f, 0.1f, "%.3f");
    ImGui::SliderFloat(" Selection Radius", &mvd_sel_radius_fact, 2.0f, 20.0f, "%.1f");
    ImGui::SliderFloat(" Selection Distance", &Editor::med_sel_dist, 0.05f, 2.0f, "%.2f");
    ImGui::PopItemWidth();
  
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);
    ImGui::InputFloat(" Grid Unit Size", &grid_unit_size, 0.1f, 1.0f, "%.2f");
    grid_unit_size = MathUtil::Clamp(grid_unit_size, 0.0f, 500.0f);
    ImGui::InputInt(" Grid Size", &grid_size);
    grid_size = (int)MathUtil::Clamp((float)grid_size, 0.0f, 5000.0f);
    ImGui::PopItemWidth();

    if (ImGui::Button("Reset Parameters")) {
      light_intensity = 0.7f;
      grid_size = 20;
      grid_unit_size = 0.5f;
      camera.sensitivity = 5.0f;
      mvd_size = 0.01f;
      mvd_sel_radius_fact = 8.0f;
      Editor::med_sel_dist = 0.3f;

      Editor::mdl_glb_rot[0] = 0;
      Editor::mdl_glb_rot[1] = 0;
      Editor::mdl_glb_rot[2] = 0;
      Editor::mdl_glb_rot[3] = 0;
     
      Editor::mdl_glb_position[0] = 0.0f;
      Editor::mdl_glb_position[1] = 0.0f;
      Editor::mdl_glb_position[2] = 0.0f;
      Editor::mdl_glb_position[3] = 0.0f;
      
      Editor::mdl_glb_scale[0] = 1.0f;
      Editor::mdl_glb_scale[1] = 1.0f;
      Editor::mdl_glb_scale[2] = 1.0f;
      Editor::mdl_glb_scale[3] = 1.0f;
    }

    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem("Colors")) {
    
    CoCadUI::PopColorPicker("Background Color", bg_col);
    CoCadUI::PopColorPicker("Model Color", i_mdl_color);
    CoCadUI::PopColorPicker("Not Selected Color", i_dvert_color);
    CoCadUI::PopColorPicker("Selected Color", i_svert_color);


    if (ImGui::Button("Reset Colors")) {
      bg_col = CoCadUI::GlmVec3ToImVec4(config_data["glb_bg"]);
      i_mdl_color = CoCadUI::GlmVec3ToImVec4(config_data["glb_mdl_color"]);
      i_dvert_color = CoCadUI::GlmVec3ToImVec4(config_data["edit_vert_desel"]);
      i_svert_color = CoCadUI::GlmVec3ToImVec4(config_data["edit_vert_sel"]);
    }

    ImGui::EndTabItem();
  }

  ImGui::EndTabBar();
  ImGui::PopStyleVar();
  CoCadUI::WindowEnd();
 
  // Chat Window
  ImGui::SetNextWindowPos(ImVec2(window_locs["win_chat"].x, window_locs["win_chat"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_chat"].x, window_sizes["win_chat"].y));

  CoCadUI::WindowStart(ICON_FA_MESSAGE " Chat");

  CoCadUI::WindowEnd();
 
  ImGui::SetNextWindowPos(ImVec2(window_locs["win_sesh"].x, window_locs["win_sesh"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_sesh"].x, window_sizes["win_sesh"].y));

  CoCadUI::WindowStart("Session Settings");

  if (sesh_info.host_exists == false) {
    if (ImGui::Button("Host Session")) { cc_client.RequestBecomeSH(); }
    
    if (ImGui::Button("End Session")) {
  
    }
  } else {
    if (ImGui::Button("Join Session")) {
      cc_client.RequestJoinSH(); 
    }
    
    if (sesh_info.currently_in_session) {
      if (ImGui::Button("Leave Session")) {
        
      }
    }
  }

  std::string sesh_status_info = "Session Status: ";
  std::string type = user.is_host ? "(Hosting)" : "(Connected)";
  std::string cs = sesh_info.currently_in_session ? "Running" : "Waiting"; 
  sesh_status_info = sesh_status_info + cs + " " + type;

  ImGui::Text(sesh_status_info.c_str());
  ImGui::Text("Hosted By: ");

  CoCadUI::WindowEnd();


  // ================ // UI LOGIN OVERLOAD // =================//
  ImGui::SetNextWindowPos(ImVec2(window_locs["win_login"].x, window_locs["win_login"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_login"].x, window_sizes["win_login"].y));

  if (logged_in == false) {
    CoCadUI::WindowStart("Login");
    
    ImGui::Text("Enter Username: ");
    CoCadUI::InputTextStdString("##loginusrn", &login_username); 
    ImGui::Text("Enter Password: ");
    CoCadUI::InputTextStdString("##loginpass", &login_password); 

    if (ImGui::Button("Login")) {
      cc_client.AuthenticateLogin(login_username, login_password);
    }

    CoCadUI::WindowEnd();
  }
  
  ImGui::PopFont();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::CleanWindow() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
