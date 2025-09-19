#pragma once

#include <Core/Context.h>

#include <Renderer/Config.h>

#include <vulkan/vulkan.h>
#include <optional>

namespace Renderer {
  struct Context {
    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;

    const char* appName_ = "";
    const char* engineName_ = "";

    uint32_t appVersion_ = 0;
    uint32_t engineVersion_ = 0;
    uint32_t apiVersion_ = 0;

    uint32_t windowId_ = 0;

    void Init(uint32_t windowId, const Core::Config& config = Core::defaultConfig);
    ~Context();
  };

  struct Surface {
    private:
      VkInstance instance_ = VK_NULL_HANDLE;

    public:
      VkSurfaceKHR surface_ = VK_NULL_HANDLE;
      uint32_t windowId_ = 0;

      void Init(const Context& ctx, uint32_t windowId);
      ~Surface();
  };

  struct Device {
    private:
      VkInstance instance_ = VK_NULL_HANDLE;

    public:
      VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
      VkDevice device_ = VK_NULL_HANDLE;

      VkPhysicalDeviceProperties physicalDeviceProperties_ = {};
      VkPhysicalDeviceFeatures physicalDeviceFeatures_ = {};
      
      uint32_t graphicsQueueFamilyIndex_ = 0;
      uint32_t presentQueueFamilyIndex_ = 0;

      VkQueue graphicsQueue_ = VK_NULL_HANDLE;
      VkQueue presentQueue_ = VK_NULL_HANDLE;

      void Init(const Context& ctx, const Surface& surface,const Core::Config& config = Core::defaultConfig);
      ~Device();
  };

  struct RenderPass;

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

  struct Swapchain {
    private:
      VkDevice device_;

    public:
      VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
      VkFormat imageFormat_ = VK_FORMAT_UNDEFINED;
      VkExtent2D extent_ = {};

      std::vector<VkImage> images_ = {};
      std::vector<VkImageView> imageViews_ = {};
      std::vector<VkFramebuffer> framebuffers_ = {};

      void Init(const Device& device, const Surface& surface, uint32_t windowID);
      void GetFramebuffers(const RenderPass& renderPass);
      ~Swapchain();
  };

  struct Shader {
    private:
      VkDevice device_;

    public:
      VkShaderModule shaderModule_ = VK_NULL_HANDLE;
      VkPipelineShaderStageCreateInfo stageInfo = {};

      void Init(const Core::Context& context, VkShaderStageFlagBits type, const char* path);
      ~Shader();
  };

  struct RenderPassDescription {
    std::vector<VkAttachmentDescription> attachments_;
    std::vector<VkSubpassDescription> subpasses_;
    std::vector<VkSubpassDependency> dependencies_;

    std::vector<VkAttachmentReference> colorAttachmentRefs_;
    std::vector<VkAttachmentReference> inputAttachmentRefs_;
    std::vector<VkAttachmentReference> resolveAttachmentRefs_;
    std::optional<VkAttachmentReference> depthAttachmentRef_;

    static RenderPassDescription DefaultDescription(VkFormat imageFormat, VkSampleCountFlagBits sampleCount) {
      RenderPassDescription description{};

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

  struct RenderPass {
    private:
      VkDevice device_;

    public:
      VkRenderPass renderPass_ = VK_NULL_HANDLE;
      RenderPassDescription description_ = {};

      void Init(const Core::Context& context, const std::optional<RenderPassDescription>& description = std::nullopt, const Config& config = defaultConfig);
      ~RenderPass();
  };

  struct GraphicsPipelineDescription {
    const Shader* vertexShader;
    const Shader* fragmentShader;
    const RenderPass* renderPass;
    uint32_t subpass = 0;

    std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
    };
  };

  struct Pipeline {
    private:
      VkDevice device_;

    public:
      VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
      VkPipeline pipeline_ = VK_NULL_HANDLE;

      void Init(const Core::Context& context, const GraphicsPipelineDescription& description, const Config& config = defaultConfig);
      ~Pipeline();
  };

  struct CommandPool {
    private:
      VkDevice device_;

    public:
      VkCommandPool commandPool_ = VK_NULL_HANDLE;
      uint32_t queueFamilyIndex_ = 0;

      void Init(const Device& device, uint32_t queueFamilyIndex);
      ~CommandPool();
  };

  struct CommandBuffer {
    private: 
      VkDevice device_;

    public:
      VkCommandBuffer buffer_ = VK_NULL_HANDLE;

      void Allocate(const Device& device, const CommandPool& commandPool, bool primary = true);
      void Begin();
      void End();
      void Submit(const Core::Context& context, const QueueFamilyIndices& queueFamilyIndices);
  };

  struct Semaphore {
    private:
      VkDevice device_;

    public:
      VkSemaphore semaphore_ = VK_NULL_HANDLE;

      void Init(const Core::Context& context);
      ~Semaphore();
  };

  struct Fence {
    private:
      VkDevice device_;

    public:
      VkFence fence_ = VK_NULL_HANDLE;

      void Init(const Core::Context& context, bool signaled = false);
      void Cleanup();
      ~Fence();
  };
}
