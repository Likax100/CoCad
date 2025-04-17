#include "RayCasting.h"

Ray::Ray(glm::vec3 origin, glm::vec3 dir) {
  this->ray_origin = origin;
  this->ray_direction = dir;
}

Ray::~Ray() { }

glm::vec3 Ray::GetPointAlongRay(float t) {
  return this->ray_origin + (t * this->ray_direction);
}
