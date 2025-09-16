#include <Renderer/VulkanTypes.h>

void Renderer::Fence::Init(const Device& device, bool signaled) {
  device_ = device.device_;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  if (vkCreateFence(device_, &fenceInfo, nullptr, &fence_) != VK_SUCCESS)
    throw std::runtime_error("failed to create fence!");
}

Renderer::Fence::~Fence() {
  if (fence_ != VK_NULL_HANDLE) {
    vkDestroyFence(device_, fence_, nullptr);
    fence_ = VK_NULL_HANDLE;
  }
}