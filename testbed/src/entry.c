#include "game.h"

#include <entry.h>

// TODO: REMOVE
#include <platform/platform.h>

b8 create_game(game *out_game) {
  
  // App configs
  out_game->app_config.start_posx = 100;
  out_game->app_config.start_posy = 100;
  out_game->app_config.start_width = 1280;
  out_game->app_config.start_height = 720;
  out_game->app_config.name = "Luminaris Testbed (Engine)";

  // Connect function pointers 
  out_game->initialize = game_initialize;
  out_game->update = game_update;
  out_game->render = game_render;
  out_game->on_resize = game_on_resize;
  
  // Create the game state
  out_game->state = platform_allocate(sizeof(game_state), FALSE);
  
  return TRUE;
}
