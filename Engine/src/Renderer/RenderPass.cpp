#include <Renderer/VulkanTypes.h>

void Renderer::RenderPass::Init(const Device& device, const Swapchain& swapchain, const std::optional<RenderPassDescription>& description, const Config& config) {
  if (!description.has_value())
    description_ = RenderPassDescription::DefaultDescription(swapchain.imageFormat_, static_cast<VkSampleCountFlagBits>(config.sampleCount));
  else
    description_ = description.value();

  VkRenderPassCreateInfo createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  createInfo.attachmentCount = static_cast<uint32_t>(description_.attachments_.size());
  createInfo.pAttachments = description_.attachments_.data();
  createInfo.subpassCount = static_cast<uint32_t>(description_.subpasses_.size());
  createInfo.pSubpasses = description_.subpasses_.data();
  createInfo.dependencyCount = static_cast<uint32_t>(description_.dependencies_.size());
  createInfo.pDependencies = description_.dependencies_.data();

  if (vkCreateRenderPass(device.device_, &createInfo, nullptr, &renderPass_) != VK_SUCCESS)
    V_FATAL("Failed to create render pass");

  device_ = device.device_;
}

Renderer::RenderPass::~RenderPass() {
  if (renderPass_ != VK_NULL_HANDLE)
    vkDestroyRenderPass(device_, renderPass_, nullptr);
}
