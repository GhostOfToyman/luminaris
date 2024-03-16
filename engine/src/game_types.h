#pragma once

#include "core/application.h"
#include "defines.h"

/**
 * Represents basic game state in a game.
 * Called for creation by the application
 **/
typedef struct game {
  // application cofig
  application_config app_config;

  // function pointer to init function
  b8 (*initialize)(struct game* game_inst);

  // function pointer to update function
  b8 (*update)(struct game* game_inst, f32 delta_time);

  // function pointer to game render function
  b8 (*render)(struct game* game_inst, f32 delta_time);

  // function pointer to handle resize
  void (*on_resize)(struct game* game_inst, u32 width, u32 height);

  // Game spacific game state. (Managed by game)
  void* state;
} game;
