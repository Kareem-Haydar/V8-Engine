#include <Core/Config.h>

#include <vulkan/vulkan.h>

Core::Config Core::defaultConfig = {
  .appName = "",
  .appVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
  .engineName = "V8 Engine",
  .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
  .apiVersion = VK_API_VERSION_1_3,
  .enableValidationLayers = true,
  .validationLayers = { "VK_LAYER_KHRONOS_validation" },
  .deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
  .physicalDeviceIndex = CHOOSE_BEST_DEVICE
};
