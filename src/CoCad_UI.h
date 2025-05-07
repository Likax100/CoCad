#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <glm/glm.hpp>

#define ImGuiChildFlags_None 0

namespace CoCadUI {

  //############################ VARS ##############################//
  typedef struct {
    bool state;
    ImVec4 color;
  } ui_ColPickerDat;

  static std::map<std::string, ui_ColPickerDat> col_pop_pickers;

  //############################ FUNCS ##############################//
  void WindowStart(const char* window_name, bool title_bar = true);
  void WindowEnd();
  void WindowFocusBorderSet(ImVec4 focus_color, ImVec4 default_color);
  void WindowFocusBorderEnd();
  void PopColorPicker(std::string label, ImVec4& default_color);
  bool InputTextStdString(const char* label, std::string* str, ImGuiInputTextFlags flags = 0);

  //######################### UTILS ################################//
  ImVec4 GlmVec3ToImVec4(glm::vec3 v);
  glm::vec3 ImVec4ToGlmVec3(ImVec4 v);
}




