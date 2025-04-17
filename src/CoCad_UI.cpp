#include "CoCad_UI.h"

namespace CoCadUI {

  void WindowStart(const char* window_name) {
    ImGui::Begin(window_name, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
  }

  void WindowEnd() { ImGui::End(); }

  void PopColorPicker(std::string label, ImVec4 default_color) {
    // Create new one if it doesnt already exist
    if (col_pop_pickers.count(label) <= 0) {
      ui_ColPickerDat cpdat;
      cpdat.state = false;
      cpdat.color = default_color;
      col_pop_pickers[label] = cpdat;
    }

    // set up and rendering of pop up, yipeee
    ImGui::Text((label + ": ").c_str());
    ImGui::SameLine();
    
    ImGui::PushID(label.c_str());
    if (ImGui::ColorButton("##color_button", col_pop_pickers[label].color, ImGuiColorEditFlags_NoTooltip)) {
      col_pop_pickers[label].state = true;
    }
    ImGui::PopID();

    if (col_pop_pickers[label].state) {
      ImGui::OpenPopup(label.c_str());
    }

    if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::ColorPicker4("Select a Color:", &col_pop_pickers[label].color.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
      if (ImGui::Button("Apply")) {
        col_pop_pickers[label].state = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  //############################# UTIL FUNCS #################################//
  ImVec4 GlmVec3ToImVec4(glm::vec3 v) {
    ImVec4 iv = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    iv.x = v.x;
    iv.y = v.y;
    iv.z = v.z;
    return iv;
  }

}
