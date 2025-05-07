#include "Animations.h"
#include <glm/glm.hpp>

// -- STATIC DEFINITIONS -- cuz needed >:|
float Animation::p_dt = 0.0f;
SpriteRenderer* Animation::p_ren = nullptr;

//-----------------------------------------
// TODO: Add safety checks - input validations and all dat

Animation::Animation() { }

void Animation::InitAnimations(SpriteRenderer* renderer) { 
  Animation::p_ren = renderer;
}

void Animation::SyncDeltaTime(float delta_time) {
  Animation::p_dt = delta_time;
}

Animation::Animation(const char* anim_sprite_sheet_path, unsigned int frames, unsigned int anim_speed_fps) {
  frame_count = frames;
  current_frame = 0;
  timer = 0.0f;
  sheet = SpriteSheet(anim_sprite_sheet_path, frames, 1);
  animation_speed = anim_speed_fps;
  p_target_ft = 1.0f / animation_speed;
}

Animation::~Animation() { }

void Animation::SetSpeed(unsigned int anim_speed_fps) {
  animation_speed = anim_speed_fps;
  p_target_ft = 1.0f / animation_speed;
  timer = 0.0f;
}

void Animation::SetSpeed(float miliseconds) {
  float new_speed = (1000.0f / miliseconds) * static_cast<float>(frame_count); 
  animation_speed = static_cast<unsigned int>(new_speed);
  p_target_ft = 1.0f / animation_speed;
  timer = 0.0f;
}

unsigned int Animation::GetSpeed() { return animation_speed; }

void Animation::p_Update() {
  timer += Animation::p_dt;
  if(timer >= p_target_ft) {
    timer -= p_target_ft;
    current_frame = (current_frame + 1);
  }
  
  if (current_frame == frame_count) {
    reached_end = true;
    current_frame = 0;
  }
}

void Animation::Play(glm::vec2 pos, glm::vec2 scale, AnimationModes mode) {
  
  // NOTE: for now assume we always want to render the first frame
  Animation::p_ren->Render(sheet, current_frame, pos, scale);

  switch (mode) {
    case ANIM_LOOP:
    reached_end = false;
    p_Update();
    break;
    case ANIM_ONCE:
    if (reached_end != true) {
      p_Update();
    } break;
  }

}

void Animation::Stop() {
  // TODO: Rethink what stop means :0 
  // aka: restart everything to initial positions or..
  // pause/stop animation at the current frame and hold it there...

  current_frame = 0;
  timer = 0.0f;
  reached_end = false;
}
