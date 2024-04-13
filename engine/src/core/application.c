#include "application.h"
#include "core/logger.h"
#include "defines.h"
#include "game_types.h"
#include "platform/platform.h"
#include "core/lmemory.h"
#include "core/event.h"
#include "core/input.h"

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

//Event handlers 
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

b8 application_create(game *game_inst) {
  if (initialized) {
    LOG_ERROR("Application already initialized.");
    return FALSE;
  }

  app_state.game_inst = game_inst;

  // init subsystems
  init_logger();
  input_initialize();

  // TODO: REMOVE
  LOG_FATAL("A test message: %f", 3.14f);
  LOG_ERROR("A test message: %f", 3.14f);
  LOG_WARN("A test message: %f", 3.14f);
  LOG_INFO("A test message: %f", 3.14f);
  LOG_DEBUG("A test message: %f", 3.14f);
  LOG_TRACE("A test message: %f", 3.14f);

  app_state.is_running = TRUE;
  app_state.is_suspended = FALSE;

  if (!event_initialize()) {
      LOG_ERROR("Event system failed initialization. Application cannot continue");
      return FALSE;
  }

  event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
  event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
  event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

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

      input_update(0);
    }
  }

  app_state.is_running = FALSE;

  // shutdown sequence
  event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
  event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
  event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
  event_shutdown();
  input_shutdown();

  platform_shutdown(&app_state.platform);

  return TRUE;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code) {
    case EVENT_CODE_APPLICATION_QUIT: {
        LOG_INFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.\n");
        app_state.is_running = FALSE;
        return TRUE;
    }
    }

    return FALSE;
}

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            // Block anything else from processing this.
            return TRUE;
        }
        else if (key_code == KEY_A) {
            // Example on checking for a key
            LOG_DEBUG("Explicit - A key pressed!");
        }
        else {
            LOG_DEBUG("'%c' key pressed in window.", key_code);
        }
    }
    else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_B) {
            // Example on checking for a key
            LOG_DEBUG("Explicit - B key released!");
        }
        else {
            LOG_DEBUG("'%c' key released in window.", key_code);
        }
    }
    return FALSE;
}