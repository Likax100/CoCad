#include "ModelLoader.h"
#include "CoCad_Utils.h"

// ============================================================= //
//                    --- .OBJ Loader ---
// ============================================================= //
std::vector<int> OBJLoader::vertex_normal_index;

Model OBJLoader::LoadModel(const char* file_path) {
  Model mdl;
  if (FileUtil::FileExists(file_path) && FileUtil::MatchesExt(file_path, ".obj")) {
    mdl.mdl_path = std::string(file_path);
    mdl.mdl_matrix = glm::mat4(1.0f);

    // Reading da file
    auto file_content = FileUtil::ReadFileLines(file_path);

    for (int i = 0; i < file_content.size(); i++) {
      auto tokens = StringUtil::SplitString(file_content[i], " ");
      
      if (tokens[0] == "o") { mdl.mdl_name = tokens[1]; }
      else if (tokens[0] == "v") {
        float v_x = std::stof(tokens[1]);
        float v_y = std::stof(tokens[2]);
        float v_z = std::stof(tokens[3]);
        mdl.unq_vertices.push_back(v_x);
        mdl.unq_vertices.push_back(v_y);
        mdl.unq_vertices.push_back(v_z);
      }

      if (tokens[0] == "vn") {
        float fn_x = std::stof(tokens[1]);
        float fn_y = std::stof(tokens[2]);
        float fn_z = std::stof(tokens[3]);
        mdl.unq_face_normals.push_back(fn_x);
        mdl.unq_face_normals.push_back(fn_y);
        mdl.unq_face_normals.push_back(fn_z);
      }

      if (tokens[0] == "f") {
        // Add triangle indices to vector
        for (int j = 1; j < tokens.size(); j++) {
          auto tri_indices = StringUtil::SplitString(tokens[j], "/");
          mdl.faces.push_back(std::stoi(tri_indices[0]) - 1);
          OBJLoader::vertex_normal_index.push_back(std::stoi(tri_indices[2]) - 1);
        }
      }
    }

    // Generate data for glDrawArrays()
    for (int v = 0; v < mdl.faces.size(); v++) {
      int base_vert_index = mdl.faces[v];
      mdl.attrib_vertices.push_back(mdl.unq_vertices[(base_vert_index*3)]);
      mdl.attrib_vertices.push_back(mdl.unq_vertices[(base_vert_index*3)+1]);
      mdl.attrib_vertices.push_back(mdl.unq_vertices[(base_vert_index*3)+2]);

      int base_norm_index = OBJLoader::vertex_normal_index[v];
      mdl.attrib_vertices.push_back(mdl.unq_face_normals[(base_norm_index*3)]);
      mdl.attrib_vertices.push_back(mdl.unq_face_normals[(base_norm_index*3)+1]);
      mdl.attrib_vertices.push_back(mdl.unq_face_normals[(base_norm_index*3)+2]);
    }

    // Generate data for edges/wireframe
    for (int e = 0; e < mdl.faces.size() / 3; e++) {
      int base_edge_index = e * 3;
      mdl.edges.push_back(mdl.faces[e]);
      mdl.edges.push_back(mdl.faces[e+1]);
      mdl.edges.push_back(mdl.faces[e+1]);
      mdl.edges.push_back(mdl.faces[e+2]);
      mdl.edges.push_back(mdl.faces[e+2]);
      mdl.edges.push_back(mdl.faces[e]);
    }

    for (int ev = 0; ev < mdl.edges.size(); ev++) {
      int base_edge_vert_index = mdl.edges[ev];
      mdl.attrib_edges.push_back(mdl.unq_vertices[(base_edge_vert_index*3)]);
      mdl.attrib_edges.push_back(mdl.unq_vertices[(base_edge_vert_index*3)+1]);
      mdl.attrib_edges.push_back(mdl.unq_vertices[(base_edge_vert_index*3)+2]);
    }

    // Clear any static variable data
    OBJLoader::vertex_normal_index.clear();

    // Return Model object
    mdl.LoadDataIntoBuffer();

  } else {
    std::cout << "ERROR-[MODEL::OBJLoader] Could not load file \"" << file_path << "\", does not exist or is of invalid type (.obj)";  
  }
  return mdl;
}

// ============================================================= //
//                     --- .STL Loader ---
// ============================================================= //
Model STLLoader::LoadModel(const char* file_path) {
  // NOTE: This loader only works with ascii stl and not byte stl files - not yet at least

  Model mdl;
  if (FileUtil::FileExists(file_path) && FileUtil::MatchesExt(file_path, ".stl")) {
    mdl.mdl_path = std::string(file_path);
    mdl.mdl_matrix = glm::mat4(1.0f);

    auto file_content = FileUtil::ReadFileLines(file_path);
  
    float curr_norm_x = 0.0f;
    float curr_norm_y = 0.0f;
    float curr_norm_z = 0.0f;

    for (int i = 0; i < file_content.size(); i++) {
      auto tokens = StringUtil::SplitString(file_content[i], " ");

      if (tokens[0] == "facet") { 
        curr_norm_x = std::stof(tokens[2]);
        curr_norm_y = std::stof(tokens[3]);
        curr_norm_z = std::stof(tokens[4]);
      }

      else if (tokens[0] == "vertex") {
        mdl.attrib_vertices.push_back(std::stof(tokens[1]));
        mdl.attrib_vertices.push_back(std::stof(tokens[2]));
        mdl.attrib_vertices.push_back(std::stof(tokens[3]));
        
        mdl.attrib_vertices.push_back(curr_norm_x);
        mdl.attrib_vertices.push_back(curr_norm_y);
        mdl.attrib_vertices.push_back(curr_norm_z);
      }
    }
  
  mdl.LoadDataIntoBuffer();

  } else {
    std::cout << "ERROR-[MODEL::STLLoader] Could not load file \"" << file_path << "\", does not exist or is of invalid type (.stl : ASCII)";  
  }
  return mdl;
}

// ============================================================= //
//                 --- Model Class Methods ---
// ============================================================= //
Model::Model() { }
Model::~Model() { }

void Model::LoadDataIntoBuffer() {
  glGenVertexArrays(1, &this->mdl_VAO);
  //glGenBuffers(1, &this->mdl_VBO);
  //glGenBuffers(1, &this->mdl_EBO);

  // Set up for glDrawElements(), a.k.a we use indexing - OBJL_V_PURE
  glBindVertexArray(this->mdl_VAO);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mdl_EBO);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->faces.size() * sizeof(unsigned int), this->faces.data(), GL_STATIC_DRAW);

  //glBindBuffer(GL_ARRAY_BUFFER, this->mdl_VBO);
  //glBufferData(GL_ARRAY_BUFFER, this->unq_vertices.size() * sizeof(float), this->unq_vertices.data(), GL_STATIC_DRAW);
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  //glEnableVertexAttribArray(0);

  // Set up for glDrawArrays(), a.k.a we dont use indexing + we have normals per vertex - OBJL_V_REDUNDANT
  glGenBuffers(1, &this->mdl_VBO_red);
  
  glBindBuffer(GL_ARRAY_BUFFER, this->mdl_VBO_red);
  glBufferData(GL_ARRAY_BUFFER, this->attrib_vertices.size() * sizeof(float), this->attrib_vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // ================== Load Edges into seperate VAO =================== //
  //glGenVertexArrays(1, &this->mdl_eVAO);
  //glGenBuffers(1, &this->mdl_eVBO);
  //glGenBuffers(1, &this->mdl_eEBO);

  //glBindVertexArray(this->mdl_eVAO);
  
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mdl_eEBO);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->edges.size() * sizeof(unsigned int), this->edges.data(), GL_STATIC_DRAW);

  //glBindBuffer(GL_ARRAY_BUFFER, this->mdl_eVBO);
  //glBufferData(GL_ARRAY_BUFFER, this->attrib_edges.size() * sizeof(float), this->attrib_edges.data(), GL_STATIC_DRAW);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  //glEnableVertexAttribArray(1);
  //glDisableVertexAttribArray(2);
}

void Model::RenderModel(Shader& shader, bool mode) {
  glBindVertexArray(this->mdl_VAO);
  if (mode == MDL_PURE) {
    glDrawElements(GL_TRIANGLES, this->faces.size(), GL_UNSIGNED_INT, 0);
  } else if (mode == MDL_REDUNDANT) {
    glDrawArrays(GL_TRIANGLES, 0, this->attrib_vertices.size() / 6);
    //glm::vec3 edge_color = glm::vec3(1.0f, 1.0f, 1.0f);
    //glBindVertexArray(this->mdl_eVAO);
    //shader.setUVec3("v3_tint", edge_color);
    //glDrawArrays(GL_LINES, 0, this->attrib_edges.size()); 
  }
  glBindVertexArray(0);
}
