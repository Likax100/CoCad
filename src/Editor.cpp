#include "Editor.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

//======================// Forward Declarations //=====================//
float Editor::s_width;
float Editor::s_height;
unsigned int Editor::sel_mode;

Camera* Editor::cam = nullptr;
EditorRepr Editor::repr;

std::vector<glm::vec3> Editor::mvd_c_positions;
std::vector<glm::vec3> Editor::mvd_instance_colors;
std::vector<glm::mat4> Editor::mvd_instance_data;
glm::vec3 Editor::mvd_glb_scale;
unsigned int Editor::mvd_c_VAO;
unsigned int Editor::mvd_c_VBO;
unsigned int Editor::mvd_instance_VBO;
unsigned int Editor::mvd_color_VBO;
bool Editor::instance_data_updated = false;
bool Editor::instance_color_updated = false;

//======================// Implementation //=====================//
void Editor::SetUp(Camera* camera, float w, float h) { 
  Editor::cam = camera;
  Editor::s_width = w;
  Editor::s_height = h;
  Editor::sel_mode = COCAD_SELMODE_NONE;

  // Set up basic mvd settings - positions + global scale
  Editor::mvd_glb_scale = glm::vec3(0.010f, 0.010f, 0.010f);
  
  for (auto i = 0; i < Editor::repr.unique_verts.size() / 3; i++) {
    float x = Editor::repr.unique_verts[(3*i)];
    float y = Editor::repr.unique_verts[(3*i)+1];
    float z = Editor::repr.unique_verts[(3*i)+2];
   
    Editor::mvd_c_positions.push_back(glm::vec3(x, y, z));

    glm::mat4 instance_mat = glm::mat4(1.0f);
    instance_mat = glm::translate(instance_mat, glm::vec3(x, y, z));
    instance_mat = glm::scale(instance_mat, Editor::mvd_glb_scale);
    Editor::mvd_instance_data.push_back(instance_mat);
  }

  // Set up instance offset buffer to enable opengl instancing - faster rendering :D
  int instance_count = Editor::mvd_c_positions.size();
  glGenBuffers(1, &Editor::mvd_instance_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_instance_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instance_count, Editor::mvd_instance_data.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // set up instance colors and buffer
  for (auto i = 0; i < Editor::mvd_c_positions.size(); i++) {
    Editor::mvd_instance_colors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
  }

  glGenBuffers(1, &Editor::mvd_color_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_color_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Editor::mvd_instance_colors.size(), Editor::mvd_instance_colors.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate MVD data to be used in rendering -- for now just do it manually
  float mvd_cube_vertices[] = {
        -1.0f, -1.0f, -1.0f, 
         1.0f, -1.0f, -1.0f,  
         1.0f,  1.0f, -1.0f, 
         1.0f,  1.0f, -1.0f, 
        -1.0f,  1.0f, -1.0f,  
        -1.0f, -1.0f, -1.0f, 

        -1.0f, -1.0f,  1.0f, 
         1.0f, -1.0f,  1.0f, 
         1.0f,  1.0f,  1.0f,  
         1.0f,  1.0f,  1.0f, 
        -1.0f,  1.0f,  1.0f, 
        -1.0f, -1.0f,  1.0f,  

        -1.0f,  1.0f,  1.0f, 
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, 
        -1.0f, -1.0f, -1.0f, 
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,

         1.0f,  1.0f,  1.0f, 
         1.0f,  1.0f, -1.0f, 
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 
         1.0f, -1.0f,  1.0f, 
         1.0f,  1.0f,  1.0f, 

        -1.0f, -1.0f, -1.0f, 
         1.0f,  1.0f, -1.0f, 
         1.0f, -1.0f,  1.0f,  
         1.0f, -1.0f,  1.0f, 
        -1.0f, -1.0f,  1.0f,  
        -1.0f, -1.0f, -1.0f, 

        -1.0f,  1.0f, -1.0f, 
         1.0f,  1.0f, -1.0f, 
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f
  };

  // Setting up MVD rendering buffers
  glGenVertexArrays(1, &Editor::mvd_c_VAO);
  glGenBuffers(1, &Editor::mvd_c_VBO);

  glBindVertexArray(Editor::mvd_c_VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_c_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(mvd_cube_vertices), mvd_cube_vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  // add instancing for mvd_c
  glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_instance_VBO);
  
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

  glVertexAttribDivisor(1, 1);
  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);

  // color instancing buffer stuff
  glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_color_VBO);
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glVertexAttribDivisor(5, 1);

  // CHECK FOR ERRORS IN VBO
  //glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_instance_VBO);
  //GLint bufferSize;
  //glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
  //if (bufferSize > 0) {
    //std::cout << "STATUS-VBO: FILLED - " << bufferSize << "\n";
  //} else {
    //std::cout << "STATUS-VBO: EMPTY" << "\n";
    // The VBO is empty
  //}

  // NOTE: VERTEX ATTRIB DIVISOR state is stored PER VAO, so its not a global state 

  glBindVertexArray(0);
}

void Editor::SetSelectionMode(unsigned int mode) { Editor::sel_mode = mode; }
unsigned int Editor::GetSelectionMode() { return Editor::sel_mode; }

void Editor::GenerateRepr(Model& mdl) {
  Editor::repr.unique_verts.clear();
  Editor::repr.unique_verts = mdl.unq_vertices;
}

void Editor::CastRay(float mouse_x, float mouse_y, glm::mat4& view_mat, glm::mat4& proj_mat) {
  // @TEST: ensure that this works for all camera orientations and vertex scalings + edges and faces
  Editor::repr.selected_verts.clear();
  std::vector<int> intersected_indices;

  // Calculate Ray Direction: 2D screen space -> 3D world space
  float norm_x = ((2.0f * mouse_x) / Editor::s_width) - 1.0f;
  float norm_y = 1.0f - ((2.0f * mouse_y) / Editor::s_height);

  glm::vec4 point_homo = glm::vec4(norm_x, norm_y, -1.0f, 1.0);
  point_homo = glm::inverse(proj_mat) * point_homo;
  glm::vec4 dir_world_space = glm::inverse(view_mat) * point_homo;
  glm::vec3 dir = glm::vec3(dir_world_space) / dir_world_space.w;
  glm::vec3 ray_direction = glm::normalize(dir - Editor::cam->pos);

  Ray ray = Ray(Editor::cam->pos, ray_direction);

  // Perform intersection tests
  // TEMPORARY SETUP
  float vert_radius = Editor::mvd_glb_scale.x * 4.0f;
  
  if (Editor::sel_mode == COCAD_SELMODE_VERT) {
    // perform ray-sphere intersection for each vertex, given radius vert_radius
    for (unsigned int v = 0; v < Editor::repr.unique_verts.size() / 3; v++) {
      float vx = Editor::repr.unique_verts[(v*3)];
      float vy = Editor::repr.unique_verts[(v*3)+1];
      float vz = Editor::repr.unique_verts[(v*3)+2];
      glm::vec3 V = glm::vec3(vx, vy, vz);

      glm::vec3 distV = V - ray.ray_origin;
      float projected_dist = glm::dot(distV, ray.ray_direction);

      float dist = glm::length(distV);

      if (projected_dist < 0) { continue; }
      else {
        float O = sqrt((dist*dist) - (projected_dist*projected_dist));
        
        if (O > vert_radius) { continue; }
        else { 
          float diff = sqrt((vert_radius*vert_radius) - (O*O)); 
          intersected_indices.push_back(v);
        }
      }
    }
  }

  // Loop through selected indices only add the closest one
  int closest_index = -1;
  float closest_distance = 999999.0f;
  bool set_first = true;
  
  for (auto sv = 0; sv < intersected_indices.size(); sv++) {
    if (set_first) {
      closest_index = intersected_indices[sv];
      closest_distance = glm::length(Editor::cam->pos - Editor::repr.unique_verts[closest_index]);
      set_first = false;
    } else {
      float distance = glm::length(Editor::cam->pos - Editor::repr.unique_verts[intersected_indices[sv]]);
      if (distance < closest_distance) {
        closest_distance = distance;
        closest_index = intersected_indices[sv];
      }
    }
  }

  if (intersected_indices.size() > 0) {
    Editor::repr.selected_verts.push_back(closest_index);
    std::cout << "SELECTED: " << closest_index << " - " << closest_distance << "\n";
  }

  Editor::instance_color_updated = true;

}

void Editor::RenderVertOverlay(Shader& sh) {
  
  if (Editor::instance_data_updated) {
    Editor::mvd_instance_data.clear();

    for (auto p = 0; p < Editor::mvd_c_positions.size(); p++) {
      glm::mat4 u_instance_mat = glm::mat4(1.0f);
      u_instance_mat = glm::translate(u_instance_mat, Editor::mvd_c_positions[p]);
      u_instance_mat = glm::scale(u_instance_mat, Editor::mvd_glb_scale);
      Editor::mvd_instance_data.push_back(u_instance_mat);
    }

    glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * Editor::mvd_c_positions.size(), Editor::mvd_instance_data.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Editor::instance_data_updated = false;
  }

  if (Editor::instance_color_updated) {
    Editor::mvd_instance_colors.clear();

    for (auto ci = 0; ci < Editor::mvd_c_positions.size(); ci++) {
      if (ci == Editor::repr.selected_verts[0]) {
        Editor::mvd_instance_colors.push_back(Editor::repr.sel_color);
      } else {
        Editor::mvd_instance_colors.push_back(Editor::repr.desel_color);
      }
    }

    glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_color_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Editor::mvd_instance_colors.size(), Editor::mvd_instance_colors.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Editor::instance_color_updated = false;
  }

  sh.Use();
  glBindVertexArray(Editor::mvd_c_VAO);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 36, Editor::mvd_c_positions.size());
}

void Editor::UpdateVertOverlayInstanceScale(float adjust_val) {
  if (Editor::cam->GetZoomDistance() <= -17.0f || Editor::cam->GetZoomDistance() >= 3.0f) { return; }

  Editor::mvd_glb_scale.x = MathUtil::Clamp(Editor::mvd_glb_scale.x + adjust_val, 0.000001f, 0.012f); 
  Editor::mvd_glb_scale.y = MathUtil::Clamp(Editor::mvd_glb_scale.y + adjust_val, 0.000001f, 0.012f); 
  Editor::mvd_glb_scale.z = MathUtil::Clamp(Editor::mvd_glb_scale.z + adjust_val, 0.000001f, 0.012f); 
  Editor::instance_data_updated = true;
}

