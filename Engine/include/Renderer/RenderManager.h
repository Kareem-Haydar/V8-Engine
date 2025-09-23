#pragma once

#include <Renderer/Renderer.h>

#include <unordered_map>

namespace Renderer {
  class RenderManager {
    private:
      Core::Context* context_;
      std::unordered_map<uint32_t, Renderer> renderers_;
      uint32_t nextId_ = 0;

    public:
      void Init(Core::Context* context) {
        context_ = context;
      }

      void Shutdown() {
        renderers_.clear();
      }

      uint32_t CreateRenderer(const char* vertexShaderPath, const char* fragmentShaderPath, const RenderPassDescription& renderPassDesc, const Config& config = defaultConfig);
      void RemoveRenderer(uint32_t rendererId);
      Renderer* GetRenderer(uint32_t rendererId);
      void Render(uint32_t rendererId);
      void RenderAll();

      void HandleResize();
  };
}
