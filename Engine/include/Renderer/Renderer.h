#pragma once

#include "VulkanTypes.h"

namespace Renderer {
  struct RendererDescription {
    const Context& ctx;
    const Device& device;
    const Surface& surface;
    const Swapchain& swapchain;

    const Shader& vertexShader;
    const Shader& fragmentShader;

    std::vector<RenderPassDescription> renderPassDescriptions;
  };

  struct FrameConfig {
    struct PipelineUsage {
      enum class Type {
        Graphics,
        Compute,
        RayTracing
      } type = Type::Graphics;

      uint32_t pipelineIndex = 0;
    };

    struct RenderPassUsage {
      uint32_t renderPassIndex = 0;
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

      FrameConfig config;

    public:
      std::vector<RenderPass> renderPasses_;
      std::vector<Pipeline> pipelines_;
      std::unordered_map<uint32_t, CommandPool> commandPools_;
      std::vector<CommandBuffer> commandBuffers_;

      void Init(const RendererDescription& info, const Config& config = defaultConfig);
      void RenderFrame(std::optional<FrameConfig> config = std::nullopt);
  };
}
