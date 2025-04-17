#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "imgui/imgui.h"

namespace CoCadUI {

  //############################ VARS ##############################//
  typedef struct {
    bool state;
    ImVec4 color;
  } ui_ColPickerDat;

  static std::map<std::string, ui_ColPickerDat> col_pop_pickers;

  //############################ FUNCS ##############################//
  void WindowStart(const char* window_name);
  void WindowEnd();
  void PopColorPicker(std::string label, ImVec4 default_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

  //######################### UTILS ################################//
  ImVec4 GlmVec3ToImVec4(glm::vec3 v);

}




