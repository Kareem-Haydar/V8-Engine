#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <Core/WindowManager.h>

#include <optional>

namespace Renderer {
  struct Context {
    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;

    const char* appName_ = "";
    const char* engineName_ = "";

    uint32_t appVersion_ = 0;
    uint32_t engineVersion_ = 0;
    uint32_t apiVersion_ = 0;

    uint32_t windowId_ = 0;

    void Init(const char* appName, uint32_t appVersion, const char* engineName, uint32_t engineVersion, uint32_t apiVersion, uint32_t windowId);
    ~Context();
  };

  struct Surface {
    private:
      VkInstance instance_ = VK_NULL_HANDLE;

    public:
      VkSurfaceKHR surface_ = VK_NULL_HANDLE;
      uint32_t windowId_ = 0;

      void Init(const Context& ctx, uint32_t windowId);
      ~Surface();
  };

  struct Device {
    private:
      VkInstance instance_ = VK_NULL_HANDLE;

    public:
      VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
      VkDevice device_ = VK_NULL_HANDLE;

      VkPhysicalDeviceProperties physicalDeviceProperties_ = {};
      VkPhysicalDeviceFeatures physicalDeviceFeatures_ = {};
      
      uint32_t graphicsQueueFamilyIndex_ = 0;
      uint32_t presentQueueFamilyIndex_ = 0;

      VkQueue graphicsQueue_ = VK_NULL_HANDLE;
      VkQueue presentQueue_ = VK_NULL_HANDLE;

      void Init(const Context& ctx, const Surface& surface, int physicalDeviceIndex);
      ~Device();
  };

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_;
    std::optional<uint32_t> present_;

    bool IsComplete() {
      return graphics_.has_value() && present_.has_value();
    }
  };
}
