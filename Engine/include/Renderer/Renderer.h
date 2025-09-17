#pragma once

#include <variant>
#include <unordered_map>

#include <Renderer/VulkanTypes.h>

namespace Renderer {
  struct RendererDescription {
    const Context& ctx;
    const Device& device;
    const Surface& surface;
    const Swapchain& swapchain;

    const Shader& vertexShader;
    const Shader& fragmentShader;

    //std::vector<GraphicsPipelineDescription> graphicsPipelines;
    //std::vector<RenderPassDescription> renderPasses;
  };

  struct FrameConfig {
    struct PipelineUsage {
      enum class Type {
        Graphics,
        Compute,
      } type = Type::Graphics;

      VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

      struct GraphicsInfo {
        std::vector<uint32_t> subpassIndices;
      };

      struct ComputeInfo {
        // Future compute-specific configurations can be added here
      };

      std::variant<GraphicsInfo, ComputeInfo> info;

      uint32_t pipelineIndex = 0;
    };

    struct RenderPassUsage {
      uint32_t renderPassIndex = 0;
      VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
      std::vector<PipelineUsage> pipelines;
    };

    struct CommandBufferUsage {
      std::vector<RenderPassUsage> renderPasses;
      std::vector<PipelineUsage> standalonePipelines;
    };

    std::vector<CommandBufferUsage> commandBuffers;
  };

  class Renderer {
    private:
      const Context* ctx_;
      const Device* device_;
      const Surface* surface_;
      const Swapchain* swapchain_;

      uint32_t currentFrame_ = 0;

    public:
      Pipeline graphicsPipeline_;
      RenderPass renderPass_;
      std::unordered_map<uint32_t, CommandPool> commandPools_;
      std::vector<CommandBuffer> commandBuffers_;
      std::vector<Semaphore> imageAvailableSemaphores_;
      std::vector<Semaphore> renderFinishedSemaphores_;
      std::vector<Fence> inFlightFences_;

      void Init(const RendererDescription& desc, const Config& config = defaultConfig);
      void RenderFrame();
  };
}
