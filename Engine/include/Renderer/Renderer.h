#pragma once

#include <Renderer/Config.h>
#include <Core/Context.h>
#include <Scene/Scene.h>

#include <optional>

struct V8_RenderPassDescription {
  std::vector<VkAttachmentDescription> attachments_;
  std::vector<VkAttachmentReference> attachmentRefs_;
  std::vector<VkSubpassDescription> subpasses_;
  std::vector<VkSubpassDependency> dependencies_;

  static V8_RenderPassDescription Default(VkFormat imageFormat, const V8_RenderConfig& config = defaultRenderConfig) {
    V8_RenderPassDescription desc;

    desc.attachments_.resize(1);
    desc.attachments_[0].format = imageFormat;
    desc.attachments_[0].samples = static_cast<VkSampleCountFlagBits>(config.sampleCount);
    desc.attachments_[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    desc.attachments_[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    desc.attachments_[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    desc.attachments_[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    desc.attachments_[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    desc.attachments_[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    desc.attachmentRefs_.resize(1);
    desc.attachmentRefs_[0].attachment = 0;
    desc.attachmentRefs_[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    desc.subpasses_.resize(1);
    desc.subpasses_[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    desc.subpasses_[0].colorAttachmentCount = 1;
    desc.subpasses_[0].pColorAttachments = &desc.attachmentRefs_[0];

    desc.dependencies_.resize(1);
    desc.dependencies_[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    desc.dependencies_[0].dstSubpass = 0;
    desc.dependencies_[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    desc.dependencies_[0].srcAccessMask = 0;
    desc.dependencies_[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    desc.dependencies_[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

    return desc;
  }
};

class V8_Renderer {
  private:
    uint32_t currentFrame_ = 0;
    V8_Context* context_ = nullptr;
    V8_Scene* scene_ = nullptr;

  public:
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> commandBuffers_;

    std::vector<VkFramebuffer> framebuffers_;

    void Init(V8_Context& ctx, const char* vertexShaderPath, const char* fragmentShaderPath, const std::optional<V8_RenderPassDescription>& renderPassDesc = std::nullopt, const V8_RenderConfig& config = defaultRenderConfig);
    ~V8_Renderer();

    void Render();
    void HandleResize();

    void BindScene(V8_Scene& scene) {
      scene_ = &scene;
    }

    void UnbindScene() {
      scene_ = nullptr;
    }
};
