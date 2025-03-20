#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define UP_VECTOR glm::vec3(0.0f, 1.0f, 0.0f)

class Camera {
  public:
    Camera();
    ~Camera();

    glm::mat4 UpdateSphericalCameraClassic(float theta, float phi);
    glm::mat4 GetCurrentCameraView();

    void SetZoomDistance(float zoom);
    float GetZoomDistance();

    glm::vec2 rotational_pos;
  
  private:
    glm::vec3 pos;
    float zoom_d;
};
