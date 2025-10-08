#pragma once

#include <Core/Window.h>
#include <Core/Config.h>
#include <Core/Logger.h>
#include <Core/Utils.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <unordered_map>

struct V8_Context {
  private:
    void CleanupSwapchain() {
      for (auto imageView : swapchainImageViews_)
        vkDestroyImageView(device_, imageView, nullptr);

      vkDestroySwapchainKHR(device_, swapchain_, nullptr);
    }

    void CleanupSyncObjects() {
      for (size_t i = 0; i < swapchainImages_.size(); i++) {
        vkDestroySemaphore(device_, imageAvailableSemaphores_[i], nullptr);
        vkDestroySemaphore(device_, renderFinishedSemaphores_[i], nullptr);
        vkDestroyFence(device_, inFlightFences_[i], nullptr);
      }

      imageAvailableSemaphores_.clear();
      renderFinishedSemaphores_.clear();
      inFlightFences_.clear();
    }

    void CreateSwapchain();
    void CreateSyncObjects();

  public:
    bool needsResize_ = false;

    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;

    VkSurfaceKHR surface_ = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;

    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkQueue presentQueue_ = VK_NULL_HANDLE;

    uint32_t graphicsQueueFamilyIndex_ = 0;
    uint32_t presentQueueFamilyIndex_ = 0;

    VmaAllocator allocator_ = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    VkFormat swapchainImageFormat_ = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent_ = {};

    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainImageViews_;

    std::unordered_map<uint32_t, VkCommandPool> commandPools_;

    std::vector<VkSemaphore> imageAvailableSemaphores_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;
    std::vector<VkFence> inFlightFences_;

    V8_CoreConfig config_;
    V8_Window window_;

    void Init(const V8_CoreConfig& config = defaultConfig);
    ~V8_Context();

    void HandleResize(uint32_t newWidth, uint32_t newHeight);
};
