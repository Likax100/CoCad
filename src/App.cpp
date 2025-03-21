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

      if (line_tokens[0] == ".") {
        if (config_data.count(line_tokens[1]) <= 0) {
          float vec_x = std::stof(line_tokens[2]);
          float vec_y = std::stof(line_tokens[3]);
          float vec_z = std::stof(line_tokens[4]);
          config_data[line_tokens[1]] = glm::vec3(vec_x, vec_y, vec_z);
        }
      }
    }
    
    std::cout << "STATUS-[SETUPCONF] Config file data set up." << std::endl;
  } else {
    std::cout << "ERROR-[SETUPCONF] Setup file does not match extensions or does not exist.";
  }
}

void App::SetupGrid() {
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

  glGenVertexArrays(1, &this->grid_VAO);
  glGenBuffers(1, &this->grid_VBO);

  glBindVertexArray(this->grid_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, this->grid_VBO);
  glBufferData(GL_ARRAY_BUFFER, grid_vertices.size() * sizeof(float), grid_vertices.data(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  this->grid_vertex_count = grid_vertices.size() / 6;
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
  glb_shader_3D.setUVec3("v3_model_color", config_data["glb_mdl_color"]);
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
  //imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(this->window, true); 
  ImGui_ImplOpenGL3_Init("#version 460");
 
  // TEMP SOLUTION TO WINDOW SIZING AND LOCATION
  window_sizes["win_obj_prop"] = glm::vec2(400.0f, fb_height);
  window_locs["win_obj_prop"] = glm::vec2(0.0f, 0.0f);
  
  window_sizes["win_sesh"] = glm::vec2(400.0f, 400.0f);
  window_locs["win_sesh"] = glm::vec2(fb_width - window_sizes["win_sesh"].x, 0.0f);
  
  window_sizes["win_chat"] = glm::vec2(400.0f, fb_height - 400.0f);
  window_locs["win_chat"] = glm::vec2(fb_width - window_sizes["win_chat"].x, fb_height - window_sizes["win_chat"].y);

  kube = OBJLoader::LoadModel("./assets/models/monke.obj"); 
  //kube.mdl_matrix = glm::scale(kube.mdl_matrix, glm::vec3(0.5f, 0.5f, 0.5f));

  //glm::mat4 proj_ortho = glm::ortho(0.0f, (float)this->w_width, (float)this->w_height, 0.0f, -1.0f, 1.0f);
  this->SetupGrid();

  this->LimitFPS(true, 60);
}


// ============================================================= //
//                  --- INPUT SECTION ---
// ============================================================= //
void App::MouseButtonInput(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_MIDDLE) { 
    if (action == GLFW_PRESS) {
      double mouse_x, mouse_y;
      glfwGetCursorPos(this->window, &mouse_x, &mouse_y);

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
  } else { 
    keymap[button] = false;
  }

}

void App::MouseScrollInput(double xoffset, double yoffset) {
  float n_zoom = camera.GetZoomDistance() + (0.5f * (float)yoffset);
  camera.SetZoomDistance(n_zoom);
  glb_view_matrix = camera.UpdateSphericalCameraClassic(0.0f, 0.0f);
}


void App::KeyboardInput(int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) { keymap[key] = true; }
  else if (action == GLFW_RELEASE) { keymap[key] = false; }
}


// ============================================================= //
//                    --- UPDATE LOOP ---
// ============================================================= //
void App::Update() {
  glfwGetCursorPos(this->window, &drag_loc_end_x, &drag_loc_end_y);
  double dt_x = (drag_loc_end_x - drag_loc_start_x) / sensitivity;  
  double dt_y = (drag_loc_end_y - drag_loc_start_y) / sensitivity;  

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
}


// ============================================================= //
//                    --- RENDER LOOP ---
// ============================================================= //
void App::Render() {
  glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glClearColor(config_data["glb_bg"].r, config_data["glb_bg"].g, config_data["glb_bg"].b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->RenderGrid();

  glb_shader_3D.Use();
  glb_shader_3D.setUInt("ren_mode", 1);
  glm::vec3 newcol = glm::vec3(def_colorp_color[0], def_colorp_color[1], def_colorp_color[2]);
  glb_shader_3D.setUVec3("v3_model_color", newcol);
  glb_shader_3D.setUMat4("m4_view", glb_view_matrix);
  
  glb_shader_3D.setUMat4("m4_model", kube.mdl_matrix);
  kube.RenderModel(glb_shader_3D);


  // ==================== UI ========================= //
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  //ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  //ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  
  ImGui::SetNextWindowPos(ImVec2(window_locs["win_obj_prop"].x, window_locs["win_obj_prop"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_obj_prop"].x, window_sizes["win_obj_prop"].y));

  ImGui::Begin("Object Properties");
  ImGui::Text("-- Transform --");
  ImGui::Text("Location/Origin:");
  ImGui::Text("X: 0.0   Y: 0.0    Z: 0.0");
  ImGui::Text("Scale:");
  ImGui::Text("X: 1.0   Y: 1.0    Z: 1.0");
  
  ImGui::Text("-- Preview --");
  ImGui::ColorPicker3("Object Color", def_colorp_color);
  ImGui::Text(glm::to_string(camera.rotational_pos).c_str());
  ImGui::SliderFloat("Sensitivity: ", &sensitivity, 0.1f, 20.0f); 
  ImGui::End();
  ImGui::SetNextWindowPos(ImVec2(window_locs["win_chat"].x, window_locs["win_chat"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_chat"].x, window_sizes["win_chat"].y));

  ImGui::Begin("Chat");
  ImGui::Text("[User 1]: Hello, how is the project going?");
  ImGui::Text("[User 2]: Hi! its going okay :)");
  ImGui::End();
 
  ImGui::SetNextWindowPos(ImVec2(window_locs["win_sesh"].x, window_locs["win_sesh"].y));
  ImGui::SetNextWindowSize(ImVec2(window_sizes["win_sesh"].x, window_sizes["win_sesh"].y));

  ImGui::Begin("Session Settings");
  if (ImGui::Button("Host Session")) {
    std::cout << "[DEBUG] Hosting session...";
  }
  
  if (ImGui::Button("End Session")) {
    std::cout << "[DEBUG] Ending Session..";
  }
  ImGui::Text("Session ID: 027xf2");
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::CleanWindow() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
