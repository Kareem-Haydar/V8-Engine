#include <Renderer/Utils.h>
#include <Core/Logger.h>

void V8_Fence::Init(const V8_Context& context, bool signaled) {
  device_ = context.device_;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  if (vkCreateFence(device_, &fenceInfo, nullptr, &fence_) != VK_SUCCESS)
    V_FATAL("Failed to create fence");
}

V8_Fence::~V8_Fence() {
  if (fence_ != VK_NULL_HANDLE) {
    vkDestroyFence(device_, fence_, nullptr);
    fence_ = VK_NULL_HANDLE;
  }
}
