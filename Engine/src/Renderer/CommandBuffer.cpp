#include <Renderer/VulkanTypes.h>

void Renderer::CommandBuffer::Allocate(const Device& device, const CommandPool& commandPool, bool primary) {
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool.commandPool_;
  allocInfo.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device.device_, &allocInfo, &buffer_) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void Renderer::CommandBuffer::Begin() {
  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(buffer_, &beginInfo) != VK_SUCCESS)
    V_ERROR("Failed to begin recording command buffer");
}

void Renderer::CommandBuffer::End() {
  if (vkEndCommandBuffer(buffer_) != VK_SUCCESS)
    V_ERROR("Failed to record command buffer");
}
