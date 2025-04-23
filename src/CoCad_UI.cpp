#include "CoCad_UI.h"

namespace CoCadUI {

  void WindowStart(const char* window_name) {
    ImGui::Begin(window_name, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
  }

  void WindowEnd() { ImGui::End(); }

  void WindowFocusBorderSet(ImVec4 focus_color, ImVec4 default_color) {
    ImGuiWindow* win = ImGui::GetCurrentWindow();
    if (win && ImGui::IsWindowHovered()) {
      ImGui::PushStyleColor(ImGuiCol_Border, focus_color);
    } else { ImGui::PushStyleColor(ImGuiCol_Border, default_color); }
  }

  void WindowFocusBorderEnd() { ImGui::PopStyleColor(); }

  void PopColorPicker(std::string label, ImVec4& default_color) {
    // Create new one if it doesnt already exist
    if (col_pop_pickers.count(label) <= 0) {
      ui_ColPickerDat cpdat;
      cpdat.state = false;
      //cpdat.color = default_color;
      col_pop_pickers[label] = cpdat;
    }

    // set up and rendering of pop up, yipeee
    ImGui::Text((label + ": ").c_str());
    ImGui::SameLine();
    
    ImGui::PushID(label.c_str());
    // col_pop_pickers[label].color
    if (ImGui::ColorButton("##color_button", default_color, ImGuiColorEditFlags_NoTooltip)) {
      col_pop_pickers[label].state = true;
    }
    ImGui::PopID();

    if (col_pop_pickers[label].state) {
      ImGui::OpenPopup(label.c_str());
    }

    if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      // &col_pop_pickers[label].color.x
      ImGui::ColorPicker4("Select a Color:", &default_color.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
      if (ImGui::Button("Apply")) {
        col_pop_pickers[label].state = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  bool InputTextStdString(const char* label, std::string* str, ImGuiInputTextFlags flags) {
    // Add the callback flag to let us resize the string buffer dynamically
    flags |= ImGuiInputTextFlags_CallbackResize;

    return ImGui::InputText(label, str->data(), str->capacity() + 1, flags,
            [](ImGuiInputTextCallbackData* data) {
              if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                std::string* str = reinterpret_cast<std::string*>(data->UserData);
                str->resize(data->BufTextLen);
                data->Buf = str->data();
              }
              return 0;
            }, (void*)str);
  }
  
  //############################# UTIL FUNCS #################################//
  ImVec4 GlmVec3ToImVec4(glm::vec3 v) {
    ImVec4 iv = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    iv.x = v.x;
    iv.y = v.y;
    iv.z = v.z;
    return iv;
  }

  glm::vec3 ImVec4ToGlmVec3(ImVec4 v) {
    glm::vec3 iv = glm::vec3(0.0f);
    iv.x = v.x;
    iv.y = v.y;
    iv.z = v.z;
    return iv;
  }

}
