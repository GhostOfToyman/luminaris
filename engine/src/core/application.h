#pragma once 

#include "defines.h"

struct game;

// Application configuration
typedef struct application_config {
  // Starting pos x 
  i16 start_posx;

  // Starting pos y
  i16 start_posy;

  // Starting width
  i16 start_width;

  // Starting height
  i16 start_height;

  // Application window name
  char* name;

} application_config;

LAPI b8 application_create(struct game* game_inst);

LAPI b8 application_run();
