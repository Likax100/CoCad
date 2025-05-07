#pragma once

#include <iostream>
#include "SpriteSheets.h"
#include "SpriteRenderer.h"

enum AnimationModes {
  ANIM_NONE,
  ANIM_ONCE,
  ANIM_LOOP,
  ANIM_TIMES
};

class Animation {
  public:
  Animation();
  Animation(const char* anim_sprite_sheet_path, unsigned int frames, unsigned int anim_speed_fps = 60);
  ~Animation();
  
  static void InitAnimations(SpriteRenderer* renderer);
  static void SyncDeltaTime(float delta_time);

  void Play(glm::vec2 pos, glm::vec2 scale, AnimationModes mode = ANIM_ONCE); 
  void SetSpeed(unsigned int anim_speed_fps);
  void SetSpeed(float miliseconds);
  unsigned int GetSpeed();
  void Stop();

  private:
  SpriteSheet sheet;
  unsigned int frame_count;
  unsigned int animation_speed;
  int current_frame;
  float timer;
  bool reached_end;

  static SpriteRenderer* p_ren;
  static float p_dt;
  float p_target_ft;
  void p_Update();

};
