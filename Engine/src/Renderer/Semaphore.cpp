#include <Renderer/VulkanTypes.h>
#include <Core/Logger.h>

void Renderer::Semaphore::Init(const Device& device) {
  device_ = device.device_;

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &semaphore_) != VK_SUCCESS) 
    V_FATAL("Failed to create semaphore");
}

Renderer::Semaphore::~Semaphore() {
  if (semaphore_ != VK_NULL_HANDLE) {
    vkDestroySemaphore(device_, semaphore_, nullptr);
    semaphore_ = VK_NULL_HANDLE;
  }
}
