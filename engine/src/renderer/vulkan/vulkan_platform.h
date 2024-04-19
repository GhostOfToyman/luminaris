#pragma once

#include "defines.h"

struct platform_state;
struct vulkan_context;

b8 platform_create_vulkan_surface(struct platform_state* plat_state, struct vulkan_context* context);

/**
 * Appends names of required extensions for this platform to extension_names_darray
*/
void platform_get_required_extension_names(const char*** extension_names_darray); 

