#include <Renderer/Vulkan/Config.h>
#include <Core/Logger.h>

#include <string>

int Renderer::Vulkan::Config::CHOOSE_BEST_DEVICE = -1;
bool Renderer::Vulkan::Config::DEBUG_MODE = true;
std::vector<const char*> Renderer::Vulkan::Config::VALIDATION_LAYERS = {
  "VK_LAYER_KHRONOS_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::Vulkan::Config::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
  switch(messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      V_DEBUG("VULKAN: {}", std::string(pCallbackData->pMessage));
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      V_INFO("VULKAN: {}", std::string(pCallbackData->pMessage));
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      V_WARNING("VULKAN: {}", std::string(pCallbackData->pMessage));
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      V_ERROR("VULKAN: {}", std::string(pCallbackData->pMessage));
      break;
    default:
      break;
  }

  return VK_FALSE;
}
