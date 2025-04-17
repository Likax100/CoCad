#pragma once

#include <iostream>
#include <glm/glm.hpp>

// ============================================================= //
//                     --- RAY CLASS ---
// ============================================================= //
class Ray {
  public:
    Ray(glm::vec3 origin, glm::vec3 dir);
    ~Ray();

    glm::vec3 GetPointAlongRay(float t);

    glm::vec3 ray_origin;
    glm::vec3 ray_direction;
  private:
};
