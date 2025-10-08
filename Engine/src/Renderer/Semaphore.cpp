#include <Renderer/Utils.h>
#include <Core/Logger.h>

void V8_Semaphore::Init(const V8_Context& context) {
  device_ = context.device_;

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &semaphore_) != VK_SUCCESS) 
    V_FATAL("Failed to create semaphore");
}

V8_Semaphore::~V8_Semaphore() {
  if (semaphore_ != VK_NULL_HANDLE) {
    vkDestroySemaphore(device_, semaphore_, nullptr);
    semaphore_ = VK_NULL_HANDLE;
  }
}
