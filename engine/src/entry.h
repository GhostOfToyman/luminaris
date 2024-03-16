#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "core/lmemory.h"
#include "game_types.h"

// externally defined function to create game
extern b8 create_game(game *out_game);

/*
 * New main entry point of the application
 */
int main(void) {

  initialize_memory();

  // request game instance from application
  game game_inst;
  if (!create_game(&game_inst)) {
    LOG_FATAL("Failed to create game");
    return -1;
  }

  // Ensure function pointers are assigned and exist
  if (!game_inst.initialize || !game_inst.update || !game_inst.render ||
      !game_inst.on_resize) {
    LOG_FATAL("Game function pointers not assigned!");
    return -2;
  }

  // initialization
  if (!application_create(&game_inst)) {
    LOG_INFO("Application failed to create!");
    return 1;
  }

  // Begin game loop
  if (!application_run()) {
    LOG_INFO("Application did not shutdown correctly.");
    return 2;
  }

  shutdown_memory();

  return 0;
}
