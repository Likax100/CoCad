#include "Objects.h"
#include <stdexcept>

//=======================// STATIC DECLARATIONS //======================//
std::map<std::string, SpriteRenderer> Object2D::ps_SpriteRenderers;

//=======================// STATIC DEFINITIONS //========================//
void Object2D::AddSpriteRenderer(SpriteRenderer &ss, std::string name) {
  if (ps_SpriteRenderers.count(name) > 0) {
    std::string err = "ERROR-[OBJECT2D::AddSpriteRenderer] SpriteRenderer with name (" + name + ") already exists.";
    throw std::runtime_error(err);
  } else {
    ps_SpriteRenderers[name] = ss;
  }
}

//======================// MEMBER FUNCTION DEFINITIONS //===========================//
Object2D::Object2D() { }
Object2D::~Object2D() { }

Object2D::Object2D(std::string sprite_path, glm::vec2 pos) {
  this->position = pos;
  this->sprite_tex = Texture2D(sprite_path.c_str());
  this->alpha = 1.0f;
  this->tint = glm::vec3(1.0f);
  this->initial_scale = glm::vec2(sprite_tex.width, sprite_tex.height);
  this->Scale(initial_scale.x, initial_scale.y);
}

void Object2D::RotateAxis(float angle, glm::vec3 axis) {
  obj_mat = glm::translate(obj_mat, glm::vec3(0.5f * initial_scale.x, 0.5f * initial_scale.y, 0.0f)); 
  obj_mat = glm::rotate(obj_mat, glm::radians(angle), axis);
  obj_mat = glm::translate(obj_mat, glm::vec3(-0.5f * initial_scale.x, -0.5f * initial_scale.y, 0.0f)); 
}

void Object2D::Scale(float x, float y) {
  obj_mat = glm::scale(obj_mat, glm::vec3(x, y, 0.0f));  
}

void Object2D::RenderObject(std::string renderer) {
  Object2D::ps_SpriteRenderers[renderer].RenderRaw(sprite_tex, obj_mat, alpha, tint); 
}
