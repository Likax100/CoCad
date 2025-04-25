#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
  
  public:
  Window(int window_width, int window_height, const char* window_title);
  virtual ~Window();

  void Run();
  void LimitFPS(bool limit, int fps = 60);

  protected:
  virtual void InitWindow();
  virtual void Update();
  virtual void Render();
  virtual void CleanWindow();

  //Input Overrides
  virtual void MouseButtonInput(int button, int action, int mods);
  virtual void KeyboardInput(int key, int scancode, int action, int mods);
  virtual void MouseScrollInput(double xoffset, double yoffset);

  GLFWwindow* window;

  int w_width, w_height;
  std::string w_title;
  double mouse_x, mouse_y;
  double smouse_x, smouse_y;
  float monitor_scale_x, monitor_scale_y;
  
  double delta_time;
  double s_delta_time;

  private:
  static void p_FrameBufferSizeCB(GLFWwindow* win, int width, int height);
  void p_FrameBufferChanged(int width, int height);
  static void p_CenterWindow(GLFWwindow* window);
  void p_SetMonitorScale();
  static void p_MouseButtonCB(GLFWwindow* window, int button, int action, int mods);
  static void p_KeyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void p_MouseScrollCB(GLFWwindow* window, double xoffset, double yoffset);

  int p_fps;
  bool p_limit_fps;
  double p_prev_time, p_curr_time;
  int p_frame_counter;
  float p_target_frame_time;

  float p_smoothing_factor; // Adjust this value for more or less smoothing

  double mouse_tx, mouse_ty;
};


