#include "Window.h"
#include "CoCad_Utils.h"

#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <string>
#include <thread>
#include <cmath>

Window::Window(int window_width, int window_height, const char* window_title) {
  
  w_width = window_width;
  w_height = window_height;
  w_title = window_title;
  
  p_fps = 60;
  p_target_frame_time = 1.0f / p_fps;
  p_limit_fps = true;

  p_prev_time = 0.0f;
  p_curr_time = 0.0f;
  p_frame_counter = 0;
  s_delta_time = 0.0f;
  p_smoothing_factor = 0.2f;

  if (!glfwInit()) {
    std::string error_msg = "ERROR-[WINDOW::Window] Failed to Initialize GLFW";
    error_msg.append(glfwGetVersionString());
    throw std::runtime_error(error_msg.c_str());
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("ERROR-[WINDOW::Window] Failed to Create GLFW Window Context");
  } 
  glfwMakeContextCurrent(window);

  //Center window
  Window::p_CenterWindow(window);
  p_SetMonitorScale();

  //Set Default FrameRate:
  LimitFPS(true);

  //Load GLAD - TODO: add version to error string
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("ERROR-[WINDOW::Window] Failed to Initialize GLAD");
  }

  glfwSetFramebufferSizeCallback(this->window, &Window::p_FrameBufferSizeCB);
  
  glfwSetWindowUserPointer(this->window, this);
  glfwSetMouseButtonCallback(this->window, &Window::p_MouseButtonCB);
  glfwSetKeyCallback(this->window, &Window::p_KeyboardCB);
  glfwSetScrollCallback(this->window, &Window::p_MouseScrollCB);

  //Set up some basic OpenGL settings
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window() {
  CleanWindow();
  glfwDestroyWindow(window);
  glfwTerminate();
}

// TODO: Find a more elegent way of doing this? - store FBSCB as a function outside of class?
void Window::p_FrameBufferSizeCB(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);

  // Get the Window object associated with the GLFW window
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win) { win->p_FrameBufferChanged(width, height); }
}

void Window::p_FrameBufferChanged(int width, int height) {
  this->w_width = width;
  this->w_height = height;
}

// TODO: Somehow combine input and similar input callback functions into one??
void Window::p_MouseButtonCB(GLFWwindow* window, int button, int action, int mods) {
  Window* win_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win_instance) {
    win_instance->MouseButtonInput(button, action, mods);
  }
}

void Window::p_KeyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
  Window* win_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win_instance) {
    win_instance->KeyboardInput(key, scancode, action, mods);
  }
}

void Window::p_MouseScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
  Window* win_instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win_instance) {
    win_instance->MouseScrollInput(xoffset, yoffset);
  }
}

void Window::p_CenterWindow(GLFWwindow* window) {
  //Get the active main monitor
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  
  //Get the screen dimensions of monitor
  const GLFWvidmode* screen = glfwGetVideoMode(monitor);
  
  //Finally calculate the center and center the window
  int winWidth, winHeight;
  glfwGetWindowSize(window, &winWidth, &winHeight);

  int x_offset = (screen->width - winWidth) / 2;
  int y_offset = (screen->height - winHeight) / 2;

  glfwSetWindowPos(window, x_offset, y_offset);
}

void Window::p_SetMonitorScale() {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  glfwGetMonitorContentScale(monitor, &monitor_scale_x, &monitor_scale_y);
}

void Window::Run() {
  InitWindow();  

  glfwSwapInterval(0);

  while (!glfwWindowShouldClose(window)) {
    
    // Delta Time and FPS calculations
    p_curr_time = glfwGetTime();
    delta_time = p_curr_time - p_prev_time;
    p_frame_counter++;

    if (delta_time >= 1.0f / (float)p_fps) {
      std::string disp_fps = std::to_string((1.0f / delta_time) * p_frame_counter);
      std::string new_title = w_title + " [" + disp_fps + "/" + std::to_string(delta_time) + "]";
      glfwSetWindowTitle(window, new_title.c_str());
      p_prev_time = p_curr_time;
      p_frame_counter = 0;
    }
    
    // TODO: Find and compare alternative dtime smoothing methods, hard to tell how well this fixes jittering?
    s_delta_time = (1.0f - p_smoothing_factor) * s_delta_time + p_smoothing_factor * delta_time;

    // Retrieve clamped mouse position - Consider not needing to clamp position?
    glfwGetCursorPos(window, &this->mouse_x, &this->mouse_y);
    this->mouse_x = MathUtil::Clamp(this->mouse_x, 0, w_width * monitor_scale_x); 
    this->mouse_y = MathUtil::Clamp(this->mouse_y, 0, w_height * monitor_scale_y); 

    // Calculate scale adjusted mouse position 
    mouse_tx = mouse_x / (w_width * monitor_scale_x);
    mouse_ty = mouse_y / (w_height * monitor_scale_y);
    this->smouse_x = trunc(mouse_tx * (w_width));
    this->smouse_y = trunc(mouse_ty * (w_height));

    Update();
    Render();
    glfwSwapBuffers(window);
    glfwPollEvents();
    
    double frame_sleep_duration = p_target_frame_time - delta_time;
    if (frame_sleep_duration > 0) {
      std::this_thread::sleep_for(std::chrono::duration<double>(frame_sleep_duration));
    }
  }

  CleanWindow();
}

void Window::LimitFPS(bool limit, int fps) {
  this->p_fps = fps;
  this->p_target_frame_time = 1.0f / p_fps;
  this->p_limit_fps = limit;

  if (p_limit_fps) { glfwSwapInterval(1); }
  else { glfwSwapInterval(0); }
}

// Application Development Functions:
void Window::InitWindow() { }
void Window::Update() { }
void Window::Render() { }
void Window::CleanWindow() { }

// -- Inputs:
void Window::MouseButtonInput(int button, int action, int mods) { }
void Window::KeyboardInput(int key, int scancode, int action, int mods) { }
void Window::MouseScrollInput(double xoffset, double yoffset) { }
