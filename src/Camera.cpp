// ===================================================================== //
//  © 2024 - 2025 Lukas Rekasius (lr8217e@gre.ac.uk) 
//  - All Rights Reserved.
//  Student ID: 001220034
// ===================================================================== //

#include "Camera.h"
#include "CoCad_Utils.h"

Camera::Camera() { }
Camera::~Camera() { }

void Camera::SetZoomDistance(float zoom) {
  this->zoom_d = MathUtil::Clamp(zoom, -20.0f, -1.0f);
}

float Camera::GetZoomDistance() { return zoom_d; }

glm::mat4 Camera::GetCurrentCameraView() {
  return this->UpdateSphericalCameraClassic(rotational_pos.x, rotational_pos.y);
}

glm::mat4 Camera::UpdateSphericalCameraClassic(float theta, float phi) {
  //NOTE: theta = angle along xy-plane, hence this controls horizontal rotation
  //      phi = angle between point-vector and +vertical (Y) axis, hence controls vertical rotation
 
  // TODO: figure out appropriate clamping
  //phi = MathUtil::Clamp(phi, -89.0f, 89.0f);

  //convert to radians cuz rn its way too sensitive
  theta = glm::radians(theta);
  phi = glm::radians(phi);

  theta = this->rotational_pos.x + theta;
  phi = this->rotational_pos.y + phi;

  glm::mat4 new_view_mat = glm::mat4(1.0f);

  this->pos.x = this->zoom_d * sin(theta) * sin(phi);
  this->pos.y = this->zoom_d * cos(phi);
  this->pos.z = this->zoom_d * cos(theta) * sin(phi);

  new_view_mat = glm::lookAt(this->pos, glm::vec3(0.0f, 0.0f, 0.0f), UP_VECTOR);

  return new_view_mat; 
}

