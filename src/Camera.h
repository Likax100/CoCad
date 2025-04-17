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
    glm::mat4 GetCameraModelMat();
    
    void SetDefaultRotPosition(float theta, float phi);
    void SetZoomDistance(float zoom);
    float GetZoomDistance();

    glm::vec2 rotational_pos;
    bool isUpdated = false;
    float max_phi = 359.0f;
    float min_phi = 181.0f;
    float sensitivity = 5.0f;

    glm::vec3 pos;
  private:
    float zoom_d = 6.0f;
};
