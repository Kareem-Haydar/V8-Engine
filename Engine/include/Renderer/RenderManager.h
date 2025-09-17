#pragma once

#include <Core/WindowManager.h>
#include <Renderer/Renderer.h>

namespace Renderer {
  class RenderManager {
     private:
        Context* ctx_ = nullptr;
        Device* device_ = nullptr;

        std::unordered_map<uint32_t, Renderer> renderers_;

        uint32_t nextRendererId_ = 0;

      public:
        RenderManager() = default;
        ~RenderManager() = default;

        void Init(Context& ctx, Device& device) {
          ctx_ = &ctx;
          device_ = &device;
        }

        uint32_t CreateRenderer(uint32_t windowId, const Surface& surface, const Swapchain& swapchain, const Shader& vertexShader, const Shader& fragmentShader, const Config& config = defaultConfig);

        Renderer& GetRenderer(uint32_t rendererId) {
          auto it = renderers_.find(rendererId);
          if (it != renderers_.end())
            return it->second;

          V_ERROR("Renderer with ID {} does not exist.", rendererId);
        }

        void RemoveRenderer(uint32_t rendererId) {
          renderers_.erase(rendererId);
        }

        void Render(uint32_t rendererId) {
          auto it = renderers_.find(rendererId);
          if (it != renderers_.end())
            it->second.RenderFrame();
          else
            V_ERROR("Renderer with ID {} does not exist.", rendererId);
        }

        void RenderAll() {
          for (auto& [id, renderer] : renderers_)
            renderer.RenderFrame();
        }
  };

  extern RenderManager renderManager;
}