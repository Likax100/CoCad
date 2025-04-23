#include "ModelLoader.h"
#include "CoCad_Utils.h"

// ============================================================= //
//                    --- .OBJ Loader ---
// ============================================================= //
std::vector<unsigned int> OBJLoader::vertex_normal_index;

Model OBJLoader::LoadModel(const char* file_path) {
  Model mdl;
  if (FileUtil::FileExists(file_path) && FileUtil::MatchesExt(file_path, ".obj")) {
    mdl.mdl_path = std::string(file_path);
    mdl.mdl_matrix = glm::mat4(1.0f);

    // Reading da file
    auto file_content = FileUtil::ReadFileLines(file_path);

    for (int i = 0; i < file_content.size(); i++) {
      auto tokens = StringUtil::SplitString(file_content[i], " ");
      
      if (tokens[0] == "o") { mdl.mdl_name = tokens[1] + " (.OBJ)"; }
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

    // Clear any static variable data
    mdl.vert_normals = OBJLoader::vertex_normal_index;
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
        mdl.unq_face_normals.push_back(curr_norm_x);
        mdl.unq_face_normals.push_back(curr_norm_y);
        mdl.unq_face_normals.push_back(curr_norm_z);
      }

      else if (tokens[0] == "vertex") {
        float tk0 = std::stof(tokens[1]);
        float tk1 = std::stof(tokens[2]);
        float tk2 = std::stof(tokens[3]);
        mdl.attrib_vertices.push_back(tk0);
        mdl.attrib_vertices.push_back(tk1);
        mdl.attrib_vertices.push_back(tk2);
       
        // first check if vertex is in unq_vertices - if yes, add index, if not add vertex
        bool exists = false;
        for (auto uv = 0; uv < mdl.unq_vertices.size() / 3; uv++) {
          float cx = mdl.unq_vertices[(3*uv)];
          float cy = mdl.unq_vertices[(3*uv)+1];
          float cz = mdl.unq_vertices[(3*uv)+2];

          if (tk0 == cx && tk1 == cy && tk2 == cz) {
            mdl.faces.push_back(uv); // vertex exists already so just add index
            exists = true;
            break;
          }
        }

        if (exists == false) {
          mdl.unq_vertices.push_back(tk0);
          mdl.unq_vertices.push_back(tk1);
          mdl.unq_vertices.push_back(tk2);
          mdl.faces.push_back((mdl.unq_vertices.size() / 3) - 1);
        }

        mdl.attrib_vertices.push_back(curr_norm_x);
        mdl.attrib_vertices.push_back(curr_norm_y);
        mdl.attrib_vertices.push_back(curr_norm_z);

        mdl.vert_normals.push_back((mdl.unq_face_normals.size() / 3) - 1);
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
  glBindVertexArray(this->mdl_VAO);

  // Set up for glDrawArrays(), a.k.a we dont use indexing + we have normals per vertex - OBJL_V_REDUNDANT
  glGenBuffers(1, &this->mdl_VBO);
  
  glBindBuffer(GL_ARRAY_BUFFER, this->mdl_VBO);
  glBufferData(GL_ARRAY_BUFFER, this->attrib_vertices.size() * sizeof(float), this->attrib_vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void Model::RenderModel(Shader& shader) {
  glBindVertexArray(this->mdl_VAO);
  glDrawArrays(GL_TRIANGLES, 0, this->attrib_vertices.size() / 6);
  glBindVertexArray(0);
}
