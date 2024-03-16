#include "application.h"
#include "core/logger.h"
#include "defines.h"
#include "game_types.h"
#include "platform/platform.h"
#include "core/lmemory.h"

typedef struct application_state {
  game *game_inst;
  b8 is_running;
  b8 is_suspended;
  platform_state platform;
  i16 width;
  i16 height;
  i16 last_time;
} application_state;

static b8 initialized = FALSE;
static application_state app_state;

b8 application_create(game *game_inst) {
  if (initialized) {
    LOG_ERROR("Application already initialized.");
    return FALSE;
  }

  app_state.game_inst = game_inst;

  // init subsystems
  init_logger();

  // TODO: REMOVE
  LOG_FATAL("A test message: %f", 3.14f);
  LOG_ERROR("A test message: %f", 3.14f);
  LOG_WARN("A test message: %f", 3.14f);
  LOG_INFO("A test message: %f", 3.14f);
  LOG_DEBUG("A test message: %f", 3.14f);
  LOG_TRACE("A test message: %f", 3.14f);

  app_state.is_running = TRUE;
  app_state.is_suspended = FALSE;

  if (!platform_startup(&app_state.platform, game_inst->app_config.name,
                        game_inst->app_config.start_posx,
                        game_inst->app_config.start_posy,
                        game_inst->app_config.start_width,
                        game_inst->app_config.start_height)) {
    return FALSE;
  }

  // initialize game
  if (!app_state.game_inst->initialize(app_state.game_inst)) {
    LOG_FATAL("Game failed to initialize!");
    return FALSE;
  }

  app_state.game_inst->on_resize(app_state.game_inst, app_state.width,
                                 app_state.height);

  initialized = TRUE;

  return TRUE;
}

b8 application_run() {
  LOG_INFO(get_memory_usage_str());

  while (app_state.is_running) {
    if (!platform_pump_messages(&app_state.platform)) {
      app_state.is_running = FALSE;
    }
  
    if (!app_state.is_suspended) {
      if (!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
        LOG_FATAL("Game update failed. Shutting down now.");
        app_state.is_running = FALSE;
        break;
      }

      if (!app_state.game_inst->render(app_state.game_inst, (f32)0)) { 
        LOG_FATAL("Game render failed. Shuting down now.");
        app_state.is_running = FALSE;
        break;
      }
    }
  }

  app_state.is_running = FALSE;

  platform_shutdown(&app_state.platform);

  return TRUE;
}
