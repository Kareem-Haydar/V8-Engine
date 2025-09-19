#pragma once

#include <Core/Window.h>
#include <Core/Config.h>
#include <Core/Logger.h>

#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Core {
  struct Context {
    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;

    VkSurfaceKHR surface_ = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;

    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkQueue presentQueue_ = VK_NULL_HANDLE;

    uint32_t graphicsQueueFamilyIndex_ = 0;
    uint32_t presentQueueFamilyIndex_ = 0;

    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    VkFormat swapchainImageFormat_ = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent_ = {};

    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainImageViews_;

    std::unordered_map<uint32_t, VkCommandPool> commandPools_;

    std::vector<VkSemaphore> imageAvailableSemaphores_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;
    std::vector<VkFence> inFlightFences_;

    Config config_;
    Window window_;

    void Init(const Config& config = defaultConfig);
    ~Context();
  };
}