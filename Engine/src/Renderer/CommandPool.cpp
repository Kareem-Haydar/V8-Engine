#include <Renderer/VulkanTypes.h>

void Renderer::CommandPool::Init(const Device& device, uint32_t queueFamilyIndex) {
  device_ = device.device_;
  queueFamilyIndex_ = queueFamilyIndex;

  VkCommandPoolCreateInfo poolInfo {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamilyIndex;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS)
    V_FATAL("Failed to create command pool");
}

Renderer::CommandPool::~CommandPool() {
  if (commandPool_ != VK_NULL_HANDLE)
    vkDestroyCommandPool(device_, commandPool_, nullptr);
}
