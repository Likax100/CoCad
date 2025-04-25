#include "Editor.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

//======================// Forward Declarations //=====================//
float Editor::s_width;
float Editor::s_height;
unsigned int Editor::sel_mode;
bool Editor::control_key_state = false;
std::set<unsigned int> Editor::sel_unique_verts;
bool Editor::move_mode_active = false;
bool Editor::move_dat_update = false;

float Editor::mdl_glb_scale[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float Editor::mdl_glb_position[4] = {0.0f, 0.0f, 0.0f, 1.0f};
int Editor::mdl_glb_rot[4] = {0, 0, 0, 1};
bool Editor::mdl_glb_updated = false;

// -- repr vars --
Camera* Editor::cam = nullptr;
EditorRepr Editor::repr;
RenderRepr Editor::r_repr;

// -- MVD Vars --
std::vector<glm::vec3> Editor::mvd_c_positions;
std::vector<glm::vec3> Editor::og_mvdc_pos_cpy;
std::vector<glm::vec3> Editor::mvd_instance_colors;
std::vector<glm::mat4> Editor::mvd_instance_data;
glm::vec3 Editor::mvd_glb_scale;
float Editor::mvd_def_scale = 0.01f;
float Editor::mvd_sel_radius = 8.0f;
unsigned int Editor::mvd_c_VAO;
unsigned int Editor::mvd_c_VBO;
unsigned int Editor::mvd_instance_VBO;
unsigned int Editor::mvd_color_VBO;
bool Editor::instance_data_updated = false;
bool Editor::instance_color_updated = false;

// -- MED Vars --
float Editor::med_sel_dist = 0.30f;
unsigned int Editor::med_VAO;
unsigned int Editor::med_VBO;
std::vector<float> Editor::med_edge_data;
std::set<Edge> Editor::med_unique_edges;
bool Editor::edge_data_updated = false;
glm::mat4 Editor::edge_model_mat;

// TESTING VARS
bool Editor::TEST = false;

//======================// Implementation //=====================//
void Editor::SetUpMVD() {
   // Set up basic mvd settings - positions + global scale
  Editor::mvd_glb_scale = glm::vec3(mvd_def_scale, mvd_def_scale, mvd_def_scale);
  
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

  Editor::og_mvdc_pos_cpy = Editor::mvd_c_positions;

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

  glBindVertexArray(0);
}

void Editor::RecalculateMVD() {
  Editor::mvd_instance_data.clear();
  Editor::mvd_instance_colors.clear();
  Editor::mvd_c_positions.clear();

  Editor::mvd_glb_scale = glm::vec3(mvd_def_scale, mvd_def_scale, mvd_def_scale);
  
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

  Editor::og_mvdc_pos_cpy = Editor::mvd_c_positions;
  
  glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_instance_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * Editor::mvd_instance_data.size(), Editor::mvd_instance_data.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // set up instance colors and buffer
  for (auto i = 0; i < Editor::mvd_c_positions.size(); i++) {
    Editor::mvd_instance_colors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
  }

  glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_color_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Editor::mvd_instance_colors.size(), Editor::mvd_instance_colors.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::SetUpMED() {
  Editor::edge_model_mat = glm::mat4(1.0f);
  Editor::edge_model_mat = glm::scale(Editor::edge_model_mat, glm::vec3(1.0008f, 1.0008f, 1.0008f));

  // Find all unique edges (since we cannot delete or add - for now - do this only once)
  for (auto f = 0; f < Editor::repr.face_indices.size() / 3; f++) {
    unsigned int vi0 = Editor::repr.face_indices[(f*3)];
    unsigned int vi1 = Editor::repr.face_indices[(f*3)+1];
    unsigned int vi2 = Editor::repr.face_indices[(f*3)+2];
   
    unsigned int edges[6] = { vi0, vi1, vi1, vi2, vi2, vi0 };
    for (auto e = 0; e < 3; e++) {
      unsigned int indice_one = edges[(2*e)];
      unsigned int indice_two = edges[(2*e)+1];
      
      Edge edge;
      if (indice_one < indice_two) {
        edge = std::make_pair(indice_one, indice_two);
      } else { edge = std::make_pair(indice_two, indice_one); }

      Editor::med_unique_edges.insert(edge);
    }
  }

  // check that its working by printing all the unique edges found - working good now
  //for (auto ue: Editor::med_unique_edges) {
    //std::cout << "Edge: " << ue.first << ", " << ue.second << "\n"; 
  //}

  // Set up buffers to be used for rendering later on
  glGenVertexArrays(1, &Editor::med_VAO);
  glGenBuffers(1, &Editor::med_VBO);

  glBindVertexArray(Editor::med_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, med_VBO);
  
  glBufferData(GL_ARRAY_BUFFER, Editor::med_edge_data.size() * sizeof(float), Editor::med_edge_data.data(), GL_DYNAMIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  
  glBindVertexArray(0);

  Editor::edge_data_updated = true;
}

void Editor::SetUp(Camera* camera, float w, float h) { 
  Editor::cam = camera;
  Editor::s_width = w;
  Editor::s_height = h;
  Editor::sel_mode = COCAD_SELMODE_NONE;

  Editor::SetUpMVD();
  Editor::SetUpMED();
  Editor::GenRenderBufferData();

  // set up rendering buffers
  glGenVertexArrays(1, &Editor::r_repr.render_VAO);
  glGenBuffers(1, &Editor::r_repr.render_VBO);

  glBindVertexArray(Editor::r_repr.render_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, Editor::r_repr.render_VBO);
  glBufferData(GL_ARRAY_BUFFER, Editor::r_repr.glda_vertex_data.size() * sizeof(float), Editor::r_repr.glda_vertex_data.data(), GL_DYNAMIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

void Editor::SetSelectionMode(unsigned int mode) { Editor::sel_mode = mode; Editor::repr.selected_verts.clear(); }
unsigned int Editor::GetSelectionMode() { return Editor::sel_mode; }

void Editor::SetSelectionColors(glm::vec3 vert_sel_col, glm::vec3 vert_desel_col) {
  Editor::repr.sel_color = vert_sel_col;
  Editor::repr.desel_color = vert_desel_col;
  Editor::instance_color_updated = true;
}

void Editor::SetMVDScalings(float def_scale, float vert_sel_radius) { 
  Editor::mvd_def_scale = def_scale;
  Editor::mvd_sel_radius = vert_sel_radius;
  Editor::instance_data_updated = true;
}

void Editor::GenerateRepr(Model& mdl) {
  Editor::repr.unique_verts.clear();
  Editor::repr.unique_verts = mdl.unq_vertices;

  Editor::repr.og_vert_cpy.clear();
  Editor::repr.og_vert_cpy = mdl.unq_vertices;
  
  Editor::repr.unique_face_normals.clear();
  Editor::repr.unique_face_normals = mdl.unq_face_normals;
  
  Editor::repr.face_indices.clear();
  Editor::repr.face_indices = mdl.faces;
  
  Editor::repr.normals_redundant.clear();
  Editor::repr.normals_redundant = mdl.vert_normals;
}

void Editor::RecalculateNormals() {
  Editor::repr.unique_face_normals.clear();  
  Editor::repr.normals_redundant.clear();

  // loop through each face, get vertices and calculate normal via cross operation
  for (unsigned int f = 0; f < Editor::repr.face_indices.size() / 3; f++) {
    unsigned int v0_index = Editor::repr.face_indices[(f*3)];
    float v0_x = Editor::repr.unique_verts[(v0_index*3)];
    float v0_y = Editor::repr.unique_verts[(v0_index*3)+1];
    float v0_z = Editor::repr.unique_verts[(v0_index*3)+2];
    glm::vec3 v0 = glm::vec3(v0_x, v0_y, v0_z);

    unsigned int v1_index = Editor::repr.face_indices[(f*3)+1];
    float v1_x = Editor::repr.unique_verts[(v1_index*3)];
    float v1_y = Editor::repr.unique_verts[(v1_index*3)+1];
    float v1_z = Editor::repr.unique_verts[(v1_index*3)+2];
    glm::vec3 v1 = glm::vec3(v1_x, v1_y, v1_z);
    
    unsigned int v2_index = Editor::repr.face_indices[(f*3)+2];
    float v2_x = Editor::repr.unique_verts[(v2_index*3)];
    float v2_y = Editor::repr.unique_verts[(v2_index*3)+1];
    float v2_z = Editor::repr.unique_verts[(v2_index*3)+2];
    glm::vec3 v2 = glm::vec3(v2_x, v2_y, v2_z);

    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;
    glm::vec3 new_face_normal = glm::normalize(glm::cross(e1, e2));

    bool normal_already_exists = false;
    unsigned int ufn_id;
    for (auto ufn = 0; ufn < Editor::repr.unique_face_normals.size() / 3; ufn++) {
      float ufn_x = Editor::repr.unique_face_normals[(ufn*3)];
      float ufn_y = Editor::repr.unique_face_normals[(ufn*3)+1];
      float ufn_z = Editor::repr.unique_face_normals[(ufn*3)+2];

      if (ufn_x == new_face_normal.x && ufn_y == new_face_normal.y && ufn_z == new_face_normal.z) {
        normal_already_exists = true; 
        ufn_id = ufn; break;
      }
    }

    if (normal_already_exists) {
      Editor::repr.normals_redundant.push_back(ufn_id);
      Editor::repr.normals_redundant.push_back(ufn_id);
      Editor::repr.normals_redundant.push_back(ufn_id);
    } else {
      Editor::repr.unique_face_normals.push_back(new_face_normal.x);
      Editor::repr.unique_face_normals.push_back(new_face_normal.y);
      Editor::repr.unique_face_normals.push_back(new_face_normal.z);

      unsigned int new_id = (Editor::repr.unique_face_normals.size() / 3) - 1;
      Editor::repr.normals_redundant.push_back(new_id);
      Editor::repr.normals_redundant.push_back(new_id);
      Editor::repr.normals_redundant.push_back(new_id);
    }
  }

  // print for testing once:
  if (Editor::TEST) {
    
    std::cout << "UNIQUE FN [" << Editor::repr.unique_face_normals.size() << "]: \n";
    for (auto i = 0; i < Editor::repr.unique_face_normals.size() / 3; i++) {
      float x = Editor::repr.unique_face_normals[(i*3)];
      float y = Editor::repr.unique_face_normals[(i*3)+1];
      float z = Editor::repr.unique_face_normals[(i*3)+2];

      std::cout << "ufn" << i << " : " << x << " " << y << " " << z << "\n";
    }

    std::cout << "REDUNDANT FNs[" << Editor::repr.normals_redundant.size() << "]: \n"; 
    for (auto j = 0; j < Editor::repr.normals_redundant.size() / 3; j++) {
      unsigned int v0n = Editor::repr.normals_redundant[(j*3)];
      unsigned int v1n = Editor::repr.normals_redundant[(j*3)+1];
      unsigned int v2n = Editor::repr.normals_redundant[(j*3)+2];

      std::cout << "fn" << j << " : " << v0n << " " << v1n << " " << v2n << "\n";
    }

    Editor::TEST = false;
  }


}

bool Editor::MollerTriangleIntersect(Ray r, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& ray_t, float& u, float& v) {
  // Note: this is the one sided triangle version - returns true or false if ray intersects triangle
  // 1: create edge vectors from two vertices sharing a single vertex (v0)
  glm::vec3 e1 = v1 - v0;
  glm::vec3 e2 = v2 - v0;

  // 2: begin (crammers rule) with determinant calculation + set up and calc u
  glm::vec3 rd_cross_e2 = glm::cross(r.ray_direction, e2);
  float det = glm::dot(e1, rd_cross_e2);
  if (det < ray_epsilon) { return false; } // 1st optimisation check
   
  glm::vec3 v0_to_ro = r.ray_origin - v0;
  u = glm::dot(v0_to_ro, rd_cross_e2);

  // note: all barycentric coordinates have to add to one (absolute values) for the intersection point to be in the
  // tritrue rather than outside on the plane of said triangle
  if (u < 0.0f || u > det) { return false; } // 2nd optimisation check, note for 1 sided tri, determinant MUST be positive

  // 3: set up and calculate v
  glm::vec3 uv_cross_e1 = glm::cross(v0_to_ro, e1);
  v = glm::dot(r.ray_direction, uv_cross_e1);
  if (v < 0.0f || u + v > det) { return false; } // 3rd check, if this fails, point is in triangle :)
  
  // 4: calculate t value for ray, and perform scaling of parameters (finishing crammer's rule)
  ray_t = glm::dot(e2, uv_cross_e1);
  float idet = 1.0f / det;

  ray_t = ray_t * idet;
  u = u * idet;
  v = v * idet;
  return true;
}

void Editor::CastRay(float mouse_x, float mouse_y, glm::mat4& view_mat, glm::mat4& proj_mat) {
  // if currently moving vertices dont cast at all
  if (Editor::move_mode_active) { return; }

  // NOTE: for multi-select just adjust how the clearing of the selection arrays work?
  // @TEST: ensure that this works for all camera orientations and vertex scalings + edges and faces
  if (Editor::control_key_state == false) {
    Editor::repr.selected_verts.clear();
  }

  std::vector<int> intersected_indices;
  std::vector<float> inter_tri_t_values;
  bool set_first_min = true;
 
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
  float vert_radius = Editor::mvd_def_scale * Editor::mvd_sel_radius; 
 
  if (Editor::sel_mode != COCAD_SELMODE_NONE) {
      for (auto f = 0; f < Editor::repr.face_indices.size() / 3; f++) {
        // loop through all faces (each tri in form of i1 i2 i3), and perform moller intersection
        // if ray hits, select those indices as the selected verts for the face
        int v0_index = Editor::repr.face_indices[(3*f)];
        int v1_index = Editor::repr.face_indices[(3*f)+1];
        int v2_index = Editor::repr.face_indices[(3*f)+2];

        //v0 vertex
        float v0_x = Editor::repr.unique_verts[(3*v0_index)];
        float v0_y = Editor::repr.unique_verts[(3*v0_index)+1];
        float v0_z = Editor::repr.unique_verts[(3*v0_index)+2];
        glm::vec3 v0 = glm::vec3(v0_x, v0_y, v0_z);

        //v1 vertex
        float v1_x = Editor::repr.unique_verts[(3*v1_index)];
        float v1_y = Editor::repr.unique_verts[(3*v1_index)+1];
        float v1_z = Editor::repr.unique_verts[(3*v1_index)+2];
        glm::vec3 v1 = glm::vec3(v1_x, v1_y, v1_z);

        //v2 vertex
        float v2_x = Editor::repr.unique_verts[(3*v2_index)];
        float v2_y = Editor::repr.unique_verts[(3*v2_index)+1];
        float v2_z = Editor::repr.unique_verts[(3*v2_index)+2];
        glm::vec3 v2 = glm::vec3(v2_x, v2_y, v2_z);

        float ray_t, tri_u, tri_v;
        bool ray_intersects = MollerTriangleIntersect(ray, v0, v1, v2, ray_t, tri_u, tri_v);
        
        if (ray_intersects) {
          //std::cout << "Triangle Indices: " << v0_index << ", " << v1_index << ", " << v2_index << "\n";
          intersected_indices.push_back(v0_index);
          intersected_indices.push_back(v1_index);
          intersected_indices.push_back(v2_index);
          inter_tri_t_values.push_back(ray_t);
          //std::cout << "FOUND INTERSECTION";
        }
      }

      // some vertices have been possibly selected hence process them for rendering - find closest tri 
      int closest_tri[3] = {-1, -1, -1};
      float closest_t_value = 999999.0f;
      
      // set up vars for other modes
      glm::vec3 v0, v1, v2, intersection_point;
      bool triangleIntersected = inter_tri_t_values.size() > 0 ? true: false;

      if (triangleIntersected) {
        for (unsigned int tvi = 0; tvi < inter_tri_t_values.size(); tvi++) {
          if (set_first_min) {
            closest_t_value = inter_tri_t_values[0];
            closest_tri[0] = intersected_indices[0];
            closest_tri[1] = intersected_indices[1];
            closest_tri[2] = intersected_indices[2];
            set_first_min = false;
            continue;
          }

          if (inter_tri_t_values[tvi] < closest_t_value) {
            closest_tri[0] = intersected_indices[(tvi*3)];
            closest_tri[1] = intersected_indices[(tvi*3)+1];
            closest_tri[2] = intersected_indices[(tvi*3)+2];
          }
        }

        // calc vertex positions for EDGE and VERT modes
        float v0_x = Editor::repr.unique_verts[(closest_tri[0]*3)];
        float v0_y = Editor::repr.unique_verts[(closest_tri[0]*3)+1];
        float v0_z = Editor::repr.unique_verts[(closest_tri[0]*3)+2];
        v0 = glm::vec3(v0_x, v0_y, v0_z);

        float v1_x = Editor::repr.unique_verts[(closest_tri[1]*3)];
        float v1_y = Editor::repr.unique_verts[(closest_tri[1]*3)+1];
        float v1_z = Editor::repr.unique_verts[(closest_tri[1]*3)+2];
        v1 = glm::vec3(v1_x, v1_y, v1_z);

        float v2_x = Editor::repr.unique_verts[(closest_tri[2]*3)];
        float v2_y = Editor::repr.unique_verts[(closest_tri[2]*3)+1];
        float v2_z = Editor::repr.unique_verts[(closest_tri[2]*3)+2];
        v2 = glm::vec3(v2_x, v2_y, v2_z);
       
        //std::cout << "Unq Size: " << Editor::repr.unique_verts.size() << "\n";
        //std::cout << "tri: " << closest_tri[0] << " " << closest_tri[1] << " " << closest_tri[2] << "\n";
        //std::cout << glm::to_string(v0) << "\n";  
        //std::cout << glm::to_string(v1) << "\n";
        //std::cout << glm::to_string(v2) << "\n";

        // calculate ray intersection point 
        intersection_point = ray.GetPointAlongRay(closest_t_value);
      }

    // For triangle - just add the intersecting tri that was found using moller algorithm
    if (Editor::sel_mode == COCAD_SELMODE_FACE && triangleIntersected) {
      Editor::repr.selected_verts.push_back(closest_tri[0]);
      Editor::repr.selected_verts.push_back(closest_tri[1]);
      Editor::repr.selected_verts.push_back(closest_tri[2]);
    }

    // For edges - calc vectors to vertices from inter_p, take pairs (per edge) and 
    // perform dot product to find closest edge to inter_p (closest val to -1.0)
    // then project first index vector onto new actual edge vector, 
    // and finally check the distance between this projected point and the inter_p,
    // if below a certain threshold, select the edge otherwise dont
    else if (Editor::sel_mode == COCAD_SELMODE_EDGE) {
      if (triangleIntersected) {
        // keep these un-normalized for now as we will use them later, create a normalised version
        glm::vec3 s0 = v0 - intersection_point;
        glm::vec3 s1 = v1 - intersection_point;
        glm::vec3 s2 = v2 - intersection_point;
        glm::vec3 s_vectors[3] = { s0, s1, s2 };

        glm::vec3 s0_norm = glm::normalize(s0);
        glm::vec3 s1_norm = glm::normalize(s1);
        glm::vec3 s2_norm = glm::normalize(s2);

        // calculate actual edge vectors
        glm::vec3 e0 = v1 - v0;
        glm::vec3 e1 = v2 - v1;
        glm::vec3 e2 = v0 - v2;
        glm::vec3 edges[3] = { e0, e1, e2 };

        // calculate the normalized vectors similarity values (remember closer to -1 == closer to edge as vectors oppose)
        float e0_similarity = glm::dot(s0_norm, s1_norm);
        float e1_similarity = glm::dot(s1_norm, s2_norm);
        float e2_similarity = glm::dot(s2_norm, s0_norm);

        //std::cout << "e0_sim: " << e0_similarity << ", e1_sim: " << e1_similarity << ", e2_sim" << e2_similarity << "\n"; 
        //std::cout << "v0: " << glm::to_string(v0) << ", v1: " << glm::to_string(v1) << ", v2: " << glm::to_string(v2) << "\n";       
        //std::cout << "e0: " << glm::to_string(e0) << ", e1: " << glm::to_string(e1) << ", e2: " << glm::to_string(e2) << "\n";       

        float e_similarities[3] = { e0_similarity, e1_similarity, e2_similarity };

        // mini-min function for n amount of terms to find the smallest (closest to -1) edge
        unsigned int closest_edge = -1;
        float smallest_e_sim_val = 99999.0f; // ensures that any e_sim value will always be smaller than this
        for (auto ei = 0; ei < 3; ei++) {
          if (e_similarities[ei] < smallest_e_sim_val) {
            // remember: v0 has the index of closest_tri[0], and so on, we want the index later
            closest_edge = ei;
            smallest_e_sim_val = e_similarities[ei];
          }
        }

        //std::cout << "Closest Edge: " << glm::to_string(edges[closest_edge]) << "\n";

        // edge obtained - hence perform vector projection to find point on edge ray point lands on,
        // then use this to compare distance from edge with a threshold to finally decide if point
        // is close enough to edge to be selected
        // note: invert the sn vector as otherwise we may have projection issues
        // proj_v(U) (u onto v) = (dot(u, v) / len(v)^2) * v 
        glm::vec3 v = edges[closest_edge];
        glm::vec3 u = -1.0f * s_vectors[closest_edge];
        glm::vec3 projected_inter_p = (glm::dot(u, v) / (glm::length(v)*glm::length(v))) * v;

        // get perpendicular component instead = u - parallel component 
        glm::vec3 projected_parallel = u - projected_inter_p;
        float d = glm::length(projected_parallel); // finally calculate distance and check
    
        if (d <= Editor::med_sel_dist) {
          Editor::repr.selected_verts.push_back(closest_tri[closest_edge]); 
          Editor::repr.selected_verts.push_back(closest_tri[(closest_edge + 1) % 3]); 
        }
      }

      Editor::edge_data_updated = true;
    }

    // For vertices - get intersection point and compare if its within a certain radius of
    // each of the triangle vertices, no point doing loop since only 3 vertices to deal with
    else if (Editor::sel_mode == COCAD_SELMODE_VERT) {
      if (triangleIntersected) {

        // calculate all distances
        float ip_dist_v0 = glm::length(v0 - intersection_point);
        float ip_dist_v1 = glm::length(v1 - intersection_point);
        float ip_dist_v2 = glm::length(v2 - intersection_point);
     
        //std::cout << "v0 = " << glm::to_string(v0) << "\nv1 = " << glm::to_string(v1) << "\nv2 = " << glm::to_string(v2) << "\n";
        //std::cout << "intersection point: " << glm::to_string(intersection_point) << "\n";

        //std::cout << "Set Vert Radius: " << vert_radius << "\n";
        //std::cout << "v0->ip: " << ip_dist_v0 << "\n";
        //std::cout << "v1->ip: " << ip_dist_v1 << "\n";
        //std::cout << "v2->ip: " << ip_dist_v2 << "\n";
        
        if (ip_dist_v0 <= vert_radius) { 
          Editor::repr.selected_verts.push_back(closest_tri[0]);
        } else if (ip_dist_v1 <= vert_radius) {
          Editor::repr.selected_verts.push_back(closest_tri[1]);
        } else if (ip_dist_v2 <= vert_radius) {
          Editor::repr.selected_verts.push_back(closest_tri[2]);
        } 
      }

      Editor::instance_color_updated = true;
    }

  }

  /* // OLD LIMITED "X-RAY" VERT SELECTION RAY CASTING ALGORITHM - not used anymore 
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
  */
}

void Editor::RenderVertOverlay(Shader& sh) {
  
  if (Editor::instance_data_updated) {
    Editor::mvd_instance_data.clear();
    
    Editor::mvd_glb_scale = glm::vec3(Editor::mvd_def_scale, Editor::mvd_def_scale, Editor::mvd_def_scale);

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

    for (auto p = 0; p < Editor::mvd_c_positions.size(); p++) {
      bool pos_selected = false;
      for (auto ci = 0; ci < Editor::repr.selected_verts.size(); ci++) {
        if (p == Editor::repr.selected_verts[ci]) {
          pos_selected = true;
          break;
        } 
      }

      glm::vec3 col = pos_selected ? Editor::repr.sel_color : Editor::repr.desel_color;
      Editor::mvd_instance_colors.push_back(col);
    }

    glBindBuffer(GL_ARRAY_BUFFER, Editor::mvd_color_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Editor::mvd_instance_colors.size(), Editor::mvd_instance_colors.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Editor::instance_color_updated = false;
  }

  sh.Use();
  glBindVertexArray(Editor::mvd_c_VAO);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 36, Editor::mvd_c_positions.size());
  glBindVertexArray(0);
}

void Editor::GlobalModelTransform() {
  glm::mat4 X_rot = glm::mat4(1.0f);
  glm::mat4 Y_rot = glm::mat4(1.0f);
  glm::mat4 Z_rot = glm::mat4(1.0f);

  glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(mdl_glb_scale[0], mdl_glb_scale[1], mdl_glb_scale[2]));
  
  X_rot = glm::rotate(X_rot, glm::radians((float)mdl_glb_rot[0]), glm::vec3(1.0f, 0.0f, 0.0f));
  Y_rot = glm::rotate(Y_rot, glm::radians((float)mdl_glb_rot[1]), glm::vec3(0.0f, 1.0f, 0.0f));
  Z_rot = glm::rotate(Z_rot, glm::radians((float)mdl_glb_rot[2]), glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 rot_mat = Z_rot * Y_rot * X_rot;
  
  glm::vec4 translate_vec = glm::vec4(mdl_glb_position[0], mdl_glb_position[1], mdl_glb_position[2], 0.0f);
  
  // Update MVD + vertices 
  for (auto v = 0; v < Editor::repr.unique_verts.size() / 3; v++) {
    float x = Editor::repr.og_vert_cpy[(v*3)]; 
    float y = Editor::repr.og_vert_cpy[(v*3)+1]; 
    float z = Editor::repr.og_vert_cpy[(v*3)+2];
    glm::vec4 pos = glm::vec4(x, y, z, 1.0f);

    pos = rot_mat * scale_mat * pos;
    pos = pos + translate_vec;

    Editor::repr.unique_verts[(v*3)] = pos.x;
    Editor::repr.unique_verts[(v*3)+1] = pos.y;
    Editor::repr.unique_verts[(v*3)+2] = pos.z;
  }

  for (auto mvdi = 0; mvdi < Editor::mvd_c_positions.size(); mvdi++) {
    glm::vec4 pos = glm::vec4(Editor::og_mvdc_pos_cpy[mvdi].x, Editor::og_mvdc_pos_cpy[mvdi].y, Editor::og_mvdc_pos_cpy[mvdi].z, 1.0f);
    pos = rot_mat * scale_mat * pos;
    pos = pos + translate_vec;
    Editor::mvd_c_positions[mvdi] = pos;
  }

  Editor::edge_data_updated = true;
  Editor::instance_data_updated = true;
  Editor::mdl_glb_updated = false;
}

void Editor::GenRenderBufferData() {
  Editor::r_repr.glda_vertex_data.clear();
  if (Editor::mdl_glb_updated) { Editor::GlobalModelTransform(); }
  Editor::RecalculateNormals();  

  for (auto fv = 0; fv < Editor::repr.face_indices.size() / 3; fv++) {
    // vertex + normal indices
    //std::cout << "fv = " << fv << "; size = " << Editor::repr.face_indices.size() << "\n";
    int v0_index = Editor::repr.face_indices[(3*fv)];
    int v1_index = Editor::repr.face_indices[(3*fv)+1];
    int v2_index = Editor::repr.face_indices[(3*fv)+2];
    //std::cout << "vi0 = " << v0_index << ", vi1 = " << v1_index << ", vi2 = " << v2_index << "\n";

    int n0_index = Editor::repr.normals_redundant[(3*fv)];
    int n1_index = Editor::repr.normals_redundant[(3*fv)+1];
    int n2_index = Editor::repr.normals_redundant[(3*fv)+2];
    
    // color decision
    glm::vec3 face_color = Editor::r_repr.mdl_color;
    if (Editor::sel_mode == COCAD_SELMODE_FACE) {
      for (unsigned int ci = 0; ci < Editor::repr.selected_verts.size() / 3; ci++) {
        unsigned int vi0 = Editor::repr.selected_verts[(3*ci)];
        unsigned int vi1 = Editor::repr.selected_verts[(3*ci)+1];
        unsigned int vi2 = Editor::repr.selected_verts[(3*ci)+2];

        if (v0_index == vi0 && v1_index == vi1 && v2_index == vi2) {
          face_color = Editor::repr.sel_color;
          break;
        }
      }
    } 
    // adding vertices to glda array
    // vertex 0: pos, norm, col
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v0_index*3)]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v0_index*3)+1]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v0_index*3)+2]);
    
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n0_index*3)]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n0_index*3)+1]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n0_index*3)+2]);

    Editor::r_repr.glda_vertex_data.push_back(face_color.x);
    Editor::r_repr.glda_vertex_data.push_back(face_color.y);
    Editor::r_repr.glda_vertex_data.push_back(face_color.z);

    // vertex 1: pos, norm, col
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v1_index*3)]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v1_index*3)+1]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v1_index*3)+2]);
    
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n1_index*3)]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n1_index*3)+1]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n1_index*3)+2]);

    Editor::r_repr.glda_vertex_data.push_back(face_color.x);
    Editor::r_repr.glda_vertex_data.push_back(face_color.y);
    Editor::r_repr.glda_vertex_data.push_back(face_color.z);

    // vertex 2: pos, norm, col
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v2_index*3)]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v2_index*3)+1]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_verts[(v2_index*3)+2]);
    
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n2_index*3)]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n2_index*3)+1]);
    Editor::r_repr.glda_vertex_data.push_back(Editor::repr.unique_face_normals[(n2_index*3)+2]);

    Editor::r_repr.glda_vertex_data.push_back(face_color.x);
    Editor::r_repr.glda_vertex_data.push_back(face_color.y);
    Editor::r_repr.glda_vertex_data.push_back(face_color.z);
  }

  // load data into buffer once generated
  glBindVertexArray(Editor::r_repr.render_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, Editor::r_repr.render_VBO);
  glBufferData(GL_ARRAY_BUFFER, Editor::r_repr.glda_vertex_data.size() * sizeof(float), Editor::r_repr.glda_vertex_data.data(), GL_DYNAMIC_DRAW);
  glBindVertexArray(0);
}

void Editor::Render(Shader& sh) {
  Editor::GenRenderBufferData();
  sh.Use();
  glBindVertexArray(Editor::r_repr.render_VAO);
  glDrawArrays(GL_TRIANGLES, 0, Editor::r_repr.glda_vertex_data.size() / 9);
  glBindVertexArray(0);
}

void Editor::UpdateVertOverlayInstanceScale(float adjust_val) {
  if (Editor::cam->GetZoomDistance() <= -17.0f || Editor::cam->GetZoomDistance() >= 3.0f) { return; }

  Editor::mvd_glb_scale.x = MathUtil::Clamp(Editor::mvd_glb_scale.x + adjust_val, 0.000001f, 0.012f); 
  Editor::mvd_glb_scale.y = MathUtil::Clamp(Editor::mvd_glb_scale.y + adjust_val, 0.000001f, 0.012f); 
  Editor::mvd_glb_scale.z = MathUtil::Clamp(Editor::mvd_glb_scale.z + adjust_val, 0.000001f, 0.012f); 
  Editor::instance_data_updated = true;
}

void Editor::RenderEdgeOverlay(Shader& sh) {
  
  if (Editor::edge_data_updated) {
    Editor::med_edge_data.clear();

    for (auto e: Editor::med_unique_edges) {
      
      // check selected_verts in order to choose appropriate color
      bool edge_selected = false;
      for (unsigned int i = 0; i < Editor::repr.selected_verts.size() / 2; i++) {
        // since its ordered we have to check by ordering the chosen verts too
        unsigned int ev0 = Editor::repr.selected_verts[(i*2)];
        unsigned int ev1 = Editor::repr.selected_verts[(i*2)+1];
        
        if ((e.first == ev0 || e.first == ev1) && (e.second == ev0 || e.second == ev1)) {
          edge_selected = true; break;
        }
      }

      glm::vec3 col = edge_selected ? Editor::repr.sel_color : glm::vec3(0.0f);

      // add position and color data to buffer
      float e_v0_x = Editor::repr.unique_verts[(e.first*3)];  
      float e_v0_y = Editor::repr.unique_verts[(e.first*3)+1];  
      float e_v0_z = Editor::repr.unique_verts[(e.first*3)+2];  

      Editor::med_edge_data.push_back(e_v0_x);
      Editor::med_edge_data.push_back(e_v0_y);
      Editor::med_edge_data.push_back(e_v0_z);

      Editor::med_edge_data.push_back(col.x);
      Editor::med_edge_data.push_back(col.y);
      Editor::med_edge_data.push_back(col.z);

      float e_v1_x = Editor::repr.unique_verts[(e.second*3)];  
      float e_v1_y = Editor::repr.unique_verts[(e.second*3)+1];  
      float e_v1_z = Editor::repr.unique_verts[(e.second*3)+2];  

      Editor::med_edge_data.push_back(e_v1_x);
      Editor::med_edge_data.push_back(e_v1_y);
      Editor::med_edge_data.push_back(e_v1_z);
      
      Editor::med_edge_data.push_back(col.x);
      Editor::med_edge_data.push_back(col.y);
      Editor::med_edge_data.push_back(col.z);
    }

    glBindVertexArray(Editor::med_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Editor::med_VBO);
    glBufferData(GL_ARRAY_BUFFER, Editor::med_edge_data.size() * sizeof(float), Editor::med_edge_data.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);

    Editor::edge_data_updated = false;
  }

  sh.Use();
  sh.setUFloat("alpha", 1.0f);
  sh.setUMat4("m4_model", Editor::edge_model_mat);
  glLineWidth(3.0f);
  glBindVertexArray(Editor::med_VAO);
  glDrawArrays(GL_LINES, 0, Editor::med_edge_data.size() / 6);
  glBindVertexArray(0);
}

void Editor::ClearRepr() {
  Editor::repr.og_vert_cpy.clear();
  Editor::repr.unique_verts.clear();
  Editor::repr.unique_face_normals.clear();
  Editor::repr.face_indices.clear();
  Editor::repr.normals_redundant.clear();
}

void Editor::PassModKeyControl(bool state) { Editor::control_key_state = state; }
