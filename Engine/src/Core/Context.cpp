#include <Core/Context.h>

#include <set>

struct QueueFamilyIndices {
  int graphicsFamily = -1;
  int presentFamily = -1;

  bool Complete() {
    return graphicsFamily >= 0 && presentFamily >= 0;
  }
};

struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;

  bool Adequate() {
    return !formats.empty() && !presentModes.empty();
  }
};

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
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

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) 
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  else 
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) 
    func(instance, debugMessenger, pAllocator);
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
      indices.graphicsFamily = i;

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport) 
      indices.presentFamily = i;

    if (indices.Complete())
      break;

    i++;
  }

  return indices;
}

SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapchainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
      return availableFormat;
  }

  return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes, bool enableVSync) {
  if (enableVSync) {
    return VK_PRESENT_MODE_FIFO_KHR;
  } else {
    for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        return availablePresentMode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
  }
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t windowWidth, uint32_t windowHeight) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = {
      windowWidth,
      windowHeight
    };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
  }
}

uint32_t RatePhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  uint32_t score = 0;

  // Discrete GPUs have a significant performance advantage
  score += (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ? 1000 : 0;

  // Maximum possible size of textures affects graphics quality
  score += deviceProperties.limits.maxImageDimension2D;

  QueueFamilyIndices indices = FindQueueFamilies(device, surface);
  if (!indices.Complete()) return 0;

  SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device, surface);
  if (!swapchainSupport.Adequate()) return 0;

  return score;
}

VkPhysicalDevice ChooseBestPhysicalDevice(const std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR surface) {
  VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
  uint32_t bestScore = 0;

  for (const auto& device : devices) {
    uint32_t score = RatePhysicalDevice(device, surface);
    if (score > bestScore) {
      bestScore = score;
      bestDevice = device;
    }
  }

  return bestDevice;
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
}

void Core::Context::Init(const Config& config) {
  config_ = config;

  // Initialize the window
  window_.Init(config_.windowWidth, config_.windowHeight, config_.appName.c_str(), config_.resizable, config_.fullscreen);

  // Create Vulkan instance
  VkApplicationInfo appInfo {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = config_.appName.c_str();
  appInfo.applicationVersion = config_.appVersion;
  appInfo.pEngineName = config_.engineName.c_str();
  appInfo.engineVersion = config_.engineVersion;
  appInfo.apiVersion = config_.apiVersion;

  VkInstanceCreateInfo instanceInfo {};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &appInfo;

  // Get required extensions from SDL2
  uint32_t sdlExtensionCount = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(window_.Get(), &sdlExtensionCount, nullptr)) 
    V_FATAL("Failed to get the number of required Vulkan extensions from SDL: {}", SDL_GetError());

  std::vector<const char*> extensions(sdlExtensionCount);
  if (!SDL_Vulkan_GetInstanceExtensions(window_.Get(), &sdlExtensionCount, extensions.data())) 
    V_FATAL("Failed to get the required Vulkan extensions from SDL: {}", SDL_GetError());

  if (config_.enableValidationLayers) 
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  if (strcmp(PLATFORM, "macOS") == 0) {
    extensions.push_back("VK_KHR_portability_enumeration");
    instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  }

  instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  instanceInfo.ppEnabledExtensionNames = extensions.data();

  if (config_.enableValidationLayers) {
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(config_.validationLayers.size());
    instanceInfo.ppEnabledLayerNames = config_.validationLayers.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
    PopulateDebugMessengerCreateInfo(debugCreateInfo);
    instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

  } else {
    instanceInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&instanceInfo, nullptr, &instance_) != VK_SUCCESS) 
    V_FATAL("Failed to create Vulkan instance");

  V_INFO("Vulkan instance created successfully.");

  // Create Vulkan surface
  if (!SDL_Vulkan_CreateSurface(window_.Get(), instance_, &surface_)) 
    V_FATAL("Failed to create Vulkan surface: {}", SDL_GetError());

  // Select physical device
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

  if (deviceCount == 0) 
    V_FATAL("Failed to find GPUs with Vulkan support");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

  if (config_.physicalDeviceIndex != CHOOSE_BEST_DEVICE) {
    if (config_.physicalDeviceIndex < 0 || static_cast<size_t>(config_.physicalDeviceIndex) >= devices.size()) 
      V_FATAL("Invalid physical device index: {}", config_.physicalDeviceIndex);

    physicalDevice_ = devices[config_.physicalDeviceIndex];
  } else {
    physicalDevice_ = ChooseBestPhysicalDevice(devices, surface_);
  }

  QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_, surface_);
  graphicsQueueFamilyIndex_ = indices.graphicsFamily;
  presentQueueFamilyIndex_ = indices.presentFamily;

  std::set<uint32_t> uniqueQueueFamilies = {
    (uint32_t) indices.graphicsFamily,
    (uint32_t) indices.presentFamily
  };

  VkDeviceCreateInfo deviceCreateInfo {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  VkPhysicalDeviceFeatures enabledFeatures {};
  deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(uniqueQueueFamilies.size());
  float queuePriority = 1.0f;
  for (size_t i = 0; i < uniqueQueueFamilies.size(); i++) {
    uint32_t queueFamily = *std::next(uniqueQueueFamilies.begin(), i);
    queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[i].queueFamilyIndex = queueFamily;
    queueCreateInfos[i].queueCount = 1;
    queueCreateInfos[i].pQueuePriorities = &queuePriority;
  }

  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

  std::vector<const char*> deviceExtensions = config_.deviceExtensions;
  if (strcmp(PLATFORM, "macOS") == 0) 
    deviceExtensions.push_back("VK_KHR_portability_subset");
    
  deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &device_) != VK_SUCCESS) 
    V_FATAL("Failed to create logical device");

  vkGetDeviceQueue(device_, graphicsQueueFamilyIndex_, 0, &graphicsQueue_);
  vkGetDeviceQueue(device_, presentQueueFamilyIndex_, 0, &presentQueue_);

  // Create command pools
  commandPools_.reserve(uniqueQueueFamilies.size());
  for (const auto& queueFamily : uniqueQueueFamilies) {
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPools_[queueFamily]) != VK_SUCCESS) 
      V_FATAL("Failed to create command pool");
  }

  // Create swapchain
  SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(physicalDevice_, surface_);
  VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);
  VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentModes, config_.enableVSync);
  VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities, config_.windowWidth, config_.windowHeight);

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
  if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) 
    imageCount = swapchainSupport.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR swapchainCreateInfo {};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface_;
  swapchainCreateInfo.minImageCount = imageCount;
  swapchainCreateInfo.imageFormat = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  
  if (graphicsQueueFamilyIndex_ != presentQueueFamilyIndex_) {
    uint32_t queueFamilyIndices[] = { graphicsQueueFamilyIndex_, presentQueueFamilyIndex_ };
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
  }

  swapchainCreateInfo.preTransform = swapchainSupport.capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device_, &swapchainCreateInfo, nullptr, &swapchain_) != VK_SUCCESS) 
    V_FATAL("Failed to create swapchain");

  swapchainImageFormat_ = surfaceFormat.format;
  swapchainExtent_ = extent;

  // Retrieve swapchain images
  vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, nullptr);
  swapchainImages_.resize(imageCount);
  vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, swapchainImages_.data());

  // Create image views for swapchain images
  swapchainImageViews_.resize(swapchainImages_.size());
  for (size_t i = 0; i < swapchainImages_.size(); i++) {
    VkImageViewCreateInfo viewCreateInfo {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.image = swapchainImages_[i];
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.format = swapchainImageFormat_;
    viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device_, &viewCreateInfo, nullptr, &swapchainImageViews_[i]) != VK_SUCCESS) 
      V_FATAL("Failed to create image views for swapchain images");
  }

  // Create synchronization objects
  imageAvailableSemaphores_.resize(swapchainImages_.size());
  renderFinishedSemaphores_.resize(swapchainImages_.size());
  inFlightFences_.resize(swapchainImages_.size());

  VkSemaphoreCreateInfo semaphoreInfo {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  for (size_t i = 0; i < swapchainImages_.size(); i++) {
    if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS) {
      V_FATAL("Failed to create synchronization semaphores");
    }

    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(device_, &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) {
      V_FATAL("Failed to create synchronization fence");
    }
  }

  // Setup debug messenger if validation layers are enabled
  if (!config_.enableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
  PopulateDebugMessengerCreateInfo(debugCreateInfo);
  if (CreateDebugUtilsMessengerEXT(instance_, &debugCreateInfo, nullptr, &debugMessenger_) != VK_SUCCESS) 
    V_FATAL("Failed to set up debug messenger");
}

Core::Context::~Context() {
  for (size_t i = 0; i < swapchainImages_.size(); i++) {
    vkDestroySemaphore(device_, imageAvailableSemaphores_[i], nullptr);
    vkDestroySemaphore(device_, renderFinishedSemaphores_[i], nullptr);
    vkDestroyFence(device_, inFlightFences_[i], nullptr);
  }
  
  for (auto view : swapchainImageViews_)
    vkDestroyImageView(device_, view, nullptr);

  if (swapchain_ != VK_NULL_HANDLE)
    vkDestroySwapchainKHR(device_, swapchain_, nullptr);

  for (const auto& [_, pool] : commandPools_)
    vkDestroyCommandPool(device_, pool, nullptr);
  

  if (device_ != VK_NULL_HANDLE) 
    vkDestroyDevice(device_, nullptr);

  if (surface_ != VK_NULL_HANDLE) 
    vkDestroySurfaceKHR(instance_, surface_, nullptr);

  if (config_.enableValidationLayers)
    DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);

  if (instance_ != VK_NULL_HANDLE) 
    vkDestroyInstance(instance_, nullptr);
}