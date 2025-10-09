#pragma once

#include <Core/Context.h>
#include <Renderer/Config.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <optional>

struct V8_RenderPass;

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_;
  std::optional<uint32_t> present_;

  bool IsComplete() {
    return graphics_.has_value() && present_.has_value();
  }
};

struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities_ = {};
  std::vector<VkSurfaceFormatKHR> formats_ = {};
  std::vector<VkPresentModeKHR> presentModes_ = {};
};

struct V8_Shader {
  private:
    VkDevice device_;

  public:
    VkShaderModule shaderModule_ = VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo stageInfo = {};

    void Init(const V8_Context& context, VkShaderStageFlagBits type, const char* path);
    ~V8_Shader();
};

struct V8_RenderPassDescription {
  std::vector<VkAttachmentDescription> attachments_;
  std::vector<VkSubpassDescription> subpasses_;
  std::vector<VkSubpassDependency> dependencies_;

  std::vector<VkAttachmentReference> colorAttachmentRefs_;
  std::vector<VkAttachmentReference> inputAttachmentRefs_;
  std::vector<VkAttachmentReference> resolveAttachmentRefs_;
  std::optional<VkAttachmentReference> depthAttachmentRef_;

  static V8_RenderPassDescription DefaultDescription(VkFormat imageFormat, VkSampleCountFlagBits sampleCount) {
    V8_RenderPassDescription description{};

    description.attachments_.resize(1);
    VkAttachmentDescription& attachment = description.attachments_[0];
    attachment.flags = 0;
    attachment.format = imageFormat;
    attachment.samples = sampleCount;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    description.colorAttachmentRefs_.resize(1);
    VkAttachmentReference& colorRef = description.colorAttachmentRefs_[0];
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    description.subpasses_.resize(1);
    VkSubpassDescription& subpass = description.subpasses_[0];
    subpass.flags = 0;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(description.colorAttachmentRefs_.size());
    subpass.pColorAttachments = description.colorAttachmentRefs_.data();
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = nullptr;

    description.dependencies_.resize(1);
    VkSubpassDependency& dependency = description.dependencies_[0];
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    return description;
  }
};

struct V8_RenderPass {
  private:
    VkDevice device_;

  public:
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    V8_RenderPassDescription description_ = {};

    void Init(const V8_Context& context, const std::optional<V8_RenderPassDescription>& description = std::nullopt, const V8_RenderConfig& config = defaultRenderConfig);
    ~V8_RenderPass();
};

struct GraphicsPipelineDescription {
  const V8_Shader* vertexShader;
  const V8_Shader* fragmentShader;
  const V8_RenderPass* renderPass;
  uint32_t subpass = 0;

  std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };
};

struct V8_Pipeline {
  private:
    VkDevice device_;

  public:
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkPipeline pipeline_ = VK_NULL_HANDLE;

    void Init(const V8_Context& context, const GraphicsPipelineDescription& description, const V8_RenderConfig& config = defaultRenderConfig);
    ~V8_Pipeline();
};

struct V8_CommandBuffer {
  private: 
    VkDevice device_;

  public:
    VkCommandBuffer buffer_ = VK_NULL_HANDLE;

    void Allocate(V8_Context& context, uint32_t queueFamilyIndex, bool primary = true);
    void Begin();
    void End();
    void Submit(const V8_Context& context, const QueueFamilyIndices& queueFamilyIndices);
};

struct V8_Semaphore {
  private:
    VkDevice device_;

  public:
    VkSemaphore semaphore_ = VK_NULL_HANDLE;

    void Init(const V8_Context& context);
    ~V8_Semaphore();
};

struct V8_Fence {
  private:
    VkDevice device_;

  public:
    VkFence fence_ = VK_NULL_HANDLE;

    void Init(const V8_Context& context, bool signaled = false);
    ~V8_Fence();
};

struct V8_UBOBase {
  VkBuffer buffer_ = VK_NULL_HANDLE;
  VmaAllocation allocation_ = VK_NULL_HANDLE;

  size_t size_ = 0;
};
