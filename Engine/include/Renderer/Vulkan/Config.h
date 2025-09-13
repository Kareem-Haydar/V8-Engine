#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Renderer {
  namespace Vulkan {
    namespace Config {
      extern int CHOOSE_BEST_DEVICE;
      extern bool DEBUG_MODE;
      extern std::vector<const char*> VALIDATION_LAYERS;
      extern VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
          VkDebugUtilsMessageTypeFlagsEXT messageType,
          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
          void* pUserData);
    }
  }
}
