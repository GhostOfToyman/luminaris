#include "vulkan_device.h"
#include "core/logger.h"
#include "core/lstring.h"
#include "core/lmemory.h"
#include "containers/darray.h"

typedef struct vulkan_physical_device_requirements{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    // darray
    const char** device_extension_names;
    b8 sampler_anisotropy;
    b8 discrete_gpu;
} vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info {
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;

b8 select_physical_device(vulkan_context* context);

b8 physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_family_info,
    vulkan_swapchain_support_info* out_swapchain_support_info);

b8 vulkan_device_create(vulkan_context* context) {
    if (!select_physical_device(context)) {
        return FALSE;
    }

    // TODO: This is a shortcut
    return TRUE;
}

void vulkan_device_destroy(vulkan_context* context) {}

void vulkan_device_query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, vulkan_swapchain_support_info* out_support_info) {
    
    // Surface Capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device, 
        surface, 
        &out_support_info->capabilities));

    // Surface formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device, 
        surface, 
        &out_support_info->format_count, 
        0));

    if (out_support_info->format_count != 0) {
        if (!out_support_info->formats) {
            out_support_info->formats = lallocate(sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, MEM_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, 
        surface, 
        &out_support_info->format_count, 
        out_support_info->formats));
    }

    // Present modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        surface, 
        &out_support_info->present_mode_count,
        0));

    if (out_support_info->present_mode_count != 0) {
        if (!out_support_info->present_modes) {
            out_support_info->present_modes = lallocate(sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, MEM_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info->present_mode_count,
            out_support_info->present_modes
        ));
    }
    
}

b8 select_physical_device(vulkan_context* context) {
    VkPhysicalDevice physical_device = context->vulkan_device.physical_device;
    physical_device = VK_NULL_HANDLE;

    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, 0));

    if (physical_device_count == 0) {
        LOG_FATAL("Failed to find devices with Vulkan support.");
        return FALSE;
    }

    VkPhysicalDevice physical_devices[physical_device_count];
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices));
    for (u32 i = 0; i < physical_device_count; ++i) {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &device_properties);

        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &device_features);

        VkPhysicalDeviceMemoryProperties device_memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &device_memory);

        // TODO: Should be configurable as part of the engine
        vulkan_physical_device_requirements requirements = {};
        requirements.graphics = TRUE;
        requirements.present = TRUE;
        requirements.transfer = TRUE;
        // requirements.compute = TRUE;
        requirements.sampler_anisotropy = TRUE;
        requirements.discrete_gpu = TRUE;
        requirements.device_extension_names = darray_create(const char **);
        darray_push(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info = {};
        b8 result = physical_device_meets_requirements(
            physical_devices[i],
            context->surface,
            &device_properties,
            &device_features,
            &requirements,
            &queue_info,
            &context->vulkan_device.swapchain_support);

        if (result) {
            LOG_INFO("Selected device: '%s'", device_properties.deviceName);
            switch (device_properties.deviceType) {
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    LOG_INFO("GPU type is Unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    LOG_INFO("GPU type is Integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    LOG_INFO("GPU type is Discrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    LOG_INFO("GPU type is Virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    LOG_INFO("GPU type is CPU.");
                    break;
                default:
                    break;
            }

            LOG_INFO(
                "GPU Driver Version: %d.%d.%d",
                VK_API_VERSION_MAJOR(device_properties.driverVersion),
                VK_API_VERSION_MINOR(device_properties.driverVersion),
                VK_API_VERSION_PATCH(device_properties.driverVersion));
 
            LOG_INFO(
                "Vulkan API Version: %d.%d.%d",
                VK_API_VERSION_MAJOR(device_properties.apiVersion),
                VK_API_VERSION_MINOR(device_properties.apiVersion),
                VK_API_VERSION_PATCH(device_properties.apiVersion));


            for (u32 j = 0; j < device_memory.memoryHeapCount; ++j) {
                f32 memory_size_gib = (((f32)device_memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
                if (device_memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    LOG_INFO("Local GPU memory: %.2f GiB", memory_size_gib);
                } else {
                    LOG_INFO("Shared System Memory: %.2f GiB", memory_size_gib);
                }
            }
        }

        context->vulkan_device.physical_device = physical_devices[i];
        context->vulkan_device.graphics_queue_index = queue_info.graphics_family_index;
        context->vulkan_device.present_queue_index = queue_info.present_family_index;
        context->vulkan_device.transfer_queue_index = queue_info.transfer_family_index;
        // TODO: Set compute index when needed
        context->vulkan_device.properties = device_properties;
        context->vulkan_device.features = device_features;
        context->vulkan_device.memory = device_memory;
        break; 
    }
    if (!context->vulkan_device.physical_device) {
        LOG_ERROR("No physical deivces were found which meet requirements.");
        return FALSE;
    }
    
    LOG_INFO("Physical device selected.");
    return TRUE;
}

b8 physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_family_info,
    vulkan_swapchain_support_info* out_swapchain_support_info) {

        out_queue_family_info->compute_family_index = -1;
        out_queue_family_info->graphics_family_index = -1;
        out_queue_family_info->present_family_index = -1;
        out_queue_family_info->transfer_family_index = -1;

        if (requirements->discrete_gpu){
            if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                LOG_ERROR("Not a discrete GPU. Skipping...");
                return FALSE;
            }
        }

        u32 queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
        VkQueueFamilyProperties queue_families[queue_family_count];
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

        LOG_INFO("Graphics | Present | Compute | Transfer | Name");
        u8 min_transfer_score = 255;
        for (u32 i = 0; i < queue_family_count; ++i) {
           u8 current_transfer_score = 0;

           // Graphics queue
           if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_queue_family_info->graphics_family_index = i;
            ++current_transfer_score;
           }

           // Compute queue
           if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_family_info->compute_family_index = i;
            ++current_transfer_score;
           }

           // transfer queue 
           if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if (current_transfer_score <= min_transfer_score) {
                min_transfer_score = current_transfer_score;
                out_queue_family_info->transfer_family_index = i;
            }
           }

           // Present queue
           VkBool32 supports_present = FALSE;
           VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
           if (supports_present) {
            out_queue_family_info->present_family_index = i;
           }
        }
        
        LOG_INFO("       %d |       %d |       %d |        %d | %s", 
            out_queue_family_info->graphics_family_index != -1,
            out_queue_family_info->present_family_index != -1,
            out_queue_family_info->compute_family_index != -1,
            out_queue_family_info->transfer_family_index != -1,
            properties->deviceName);
        
        if ((!requirements->graphics || (requirements->graphics && out_queue_family_info->graphics_family_index != -1)) &&
            (!requirements->present || (requirements->present && out_queue_family_info->present_family_index != -1)) &&
            (!requirements->compute || (requirements->compute && out_queue_family_info->compute_family_index != -1)) &&
            (!requirements->transfer || (requirements->transfer && out_queue_family_info->transfer_family_index != -1))) {
                LOG_INFO("Device meets Vulkan queue requirements");
                LOG_TRACE("Graphics Family Index: %i", out_queue_family_info->graphics_family_index);
                LOG_TRACE("Present Family Index: %i", out_queue_family_info->present_family_index);
                LOG_TRACE("Compute Family Index: %i", out_queue_family_info->compute_family_index);
                LOG_TRACE("Transfer Family Index: %i", out_queue_family_info->transfer_family_index);


                // Swapchain support query
                vulkan_device_query_swapchain_support(device, surface, out_swapchain_support_info);

                if (out_swapchain_support_info->format_count < 1 || out_swapchain_support_info->present_mode_count < 1) {
                    if (out_swapchain_support_info->formats) {
                        lfree(out_swapchain_support_info->formats, sizeof(VkSurfaceFormatKHR) * out_swapchain_support_info->format_count, MEM_TAG_RENDERER);
                    }
                    if (out_swapchain_support_info->present_modes) {
                        lfree(out_swapchain_support_info->present_modes, sizeof(VkPresentModeKHR) * out_swapchain_support_info->present_mode_count, MEM_TAG_RENDERER);
                    }
                    LOG_INFO("Vulkan swapchain support not present, skipping device");
                    return FALSE;
                }

                // Extensions
                if (requirements->device_extension_names) {
                    u32 available_extension_count = 0;
                    VkExtensionProperties* available_extensions = 0;
                    VK_CHECK(vkEnumerateDeviceExtensionProperties(
                        device,
                        0,
                        &available_extension_count,
                        0));

                    if (available_extension_count != 0) {
                        available_extensions = lallocate(sizeof(VkExtensionProperties) * available_extension_count, MEM_TAG_RENDERER);
                        VK_CHECK(vkEnumerateDeviceExtensionProperties(
                            device,
                            0,
                            &available_extension_count,
                            available_extensions));
                        
                        u32 required_extension_count = darray_length(requirements->device_extension_names);
                        for (u32 i = 0; i < required_extension_count; ++i) {
                            b8 found = FALSE;
                            for (u32 j = 0; j < available_extension_count; ++j) {
                                if (lstreq(requirements->device_extension_names[i], available_extensions[j].extensionName)) {
                                    found = TRUE;
                                    break;
                                }
                            }

                            if (!found) {
                                LOG_INFO("Required device extension not found: %s. Skipping device...", requirements->device_extension_names[i]);
                                lfree(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEM_TAG_RENDERER);
                                return FALSE;
                            }
                        }
                    } 
                    lfree(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEM_TAG_RENDERER);
                }

                if (requirements->sampler_anisotropy && !features->samplerAnisotropy) {
                    LOG_INFO("Device does not support samplerAnisotropy. Skipping device...");
                    return FALSE;
                }

                // Meets all requirements
                return TRUE;
            }

            return FALSE;
        }
