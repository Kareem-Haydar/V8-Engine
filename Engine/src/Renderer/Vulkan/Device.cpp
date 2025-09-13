#include <Renderer/Vulkan/VulkanTypes.h>
#include <Renderer/Vulkan/Config.h>

#include <Core/Logger.h>

#include <optional>
#include <map>
#include <set>

Renderer::Vulkan::QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
  Renderer::Vulkan::QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_ = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport) {
      indices.present_ = i;
    }
  }

  return indices;
}

uint32_t RatePhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
  uint32_t score = 0;

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);

  score += properties.limits.maxImageDimension2D;
  score += properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0;

  Renderer::Vulkan::QueueFamilyIndices indices = FindQueueFamilies(device, surface);
  if (!indices.IsComplete()) return 0;

  return score;
}

VkPhysicalDevice GetBestPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<VkPhysicalDevice>& devices) {
  std::multimap<uint32_t, VkPhysicalDevice> ratedDevices;

  for (VkPhysicalDevice device : devices)
    ratedDevices.insert(std::make_pair(RatePhysicalDevice(device, surface), device));

  if (ratedDevices.rbegin()->second && ratedDevices.rbegin()->first > 0)
    return ratedDevices.rbegin()->second;

  return VK_NULL_HANDLE;
}

void Renderer::Vulkan::Device::Init(const Context& ctx, const Surface& surface, int physicalDeviceIndex) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(ctx.instance_, &deviceCount, nullptr);

  if (deviceCount == 0) 
    V_FATAL("Failed to find GPUs with Vulkan support");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(ctx.instance_, &deviceCount, devices.data());

  if (physicalDeviceIndex != Config::CHOOSE_BEST_DEVICE && (physicalDeviceIndex < 0 || physicalDeviceIndex >= deviceCount)) {
    V_ERROR("Invalid physical device index: {} out of {} devices", physicalDeviceIndex, deviceCount);
    return;
  }

  if (physicalDeviceIndex == Config::CHOOSE_BEST_DEVICE) {
    physicalDevice_ = GetBestPhysicalDevice(ctx.instance_, surface.surface_, devices);
  } else {
    physicalDevice_ = devices[physicalDeviceIndex];
  }

  if (physicalDevice_ == VK_NULL_HANDLE) 
    V_FATAL("Failed to find a suitable GPU with Vulkan support");

  QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_, surface.surface_);
  graphicsQueueFamilyIndex_ = indices.graphics_.value();
  presentQueueFamilyIndex_ = indices.present_.value();

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice_, &properties);

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(physicalDevice_, &features);

  physicalDeviceProperties_ = properties;
  physicalDeviceFeatures_ = features;

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamilyIndex_, presentQueueFamilyIndex_ };

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkDeviceCreateInfo deviceInfo {};
  deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceInfo.pQueueCreateInfos = queueCreateInfos.data();

  std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
  deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (vkCreateDevice(physicalDevice_, &deviceInfo, nullptr, &device_) != VK_SUCCESS)
    V_FATAL("Failed to create logical device");

  vkGetDeviceQueue(device_, graphicsQueueFamilyIndex_, 0, &graphicsQueue_);
  vkGetDeviceQueue(device_, presentQueueFamilyIndex_, 0, &presentQueue_);
}

Renderer::Vulkan::Device::~Device() {
  if (device_ != VK_NULL_HANDLE)
    vkDestroyDevice(device_, nullptr);
}
