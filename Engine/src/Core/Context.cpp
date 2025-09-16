#include <Renderer/VulkanTypes.h>
#include <Core/WindowManager.h>

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = Renderer::DebugCallback;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

void Renderer::Context::Init(uint32_t windowId, const Core::Config& config) {
  if (!Core::windowManager.HasWindow(windowId)) {
    V_ERROR("Invalid window id: {}", windowId);
    return;
  }

  VkApplicationInfo appInfo {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = config.appName.c_str();
  appInfo.applicationVersion = config.appVersion;
  appInfo.pEngineName = config.engineName.c_str();
  appInfo.engineVersion = config.engineVersion;
  appInfo.apiVersion = config.apiVersion;

  VkInstanceCreateInfo instanceInfo {};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &appInfo;

  SDL_Window* win = Core::windowManager.GetWindow(windowId).Get();

  uint32_t sdlExtensionCount = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(win, &sdlExtensionCount, nullptr)) {
    V_FATAL("Failed to get SDL Vulkan instance extensions: {}", SDL_GetError());
  }

  std::vector<const char*> extensions(sdlExtensionCount);
  SDL_Vulkan_GetInstanceExtensions(win, &sdlExtensionCount, extensions.data());

  if (config.enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
    PopulateDebugMessengerCreateInfo(debugCreateInfo);

    instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
  }

  if (strcmp(PLATFORM, "macOS") == 0) {
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  }

  instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  instanceInfo.ppEnabledExtensionNames = extensions.data();

  if (vkCreateInstance(&instanceInfo, nullptr, &instance_) != VK_SUCCESS) {
    V_FATAL("Failed to create Vulkan instance");
  }

  if (!config.enableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
  PopulateDebugMessengerCreateInfo(debugCreateInfo);

  if (CreateDebugUtilsMessengerEXT(instance_, &debugCreateInfo, nullptr, &debugMessenger_) != VK_SUCCESS) {
    V_FATAL("Failed to create Vulkan debug messenger");
  }
}

Renderer::Context::~Context() {
  if (DEBUG_MODE) {
    DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
  }

  vkDestroyInstance(instance_, nullptr);
}
