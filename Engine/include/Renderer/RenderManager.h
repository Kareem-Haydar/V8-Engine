#pragma once

#include <Renderer/Renderer.h>

#include <unordered_map>

class V8_RenderManager {
  private:
    V8_Context* context_;
    std::unordered_map<std::string, V8_Renderer> renderers_;
    uint32_t nextId_ = 0;

  public:
    void Init(V8_Context* context) {
      context_ = context;
    }

    void Shutdown() {
      renderers_.clear();
    }

    void CreateRenderer(const std::string& name, const char* vertexShaderPath, const char* fragmentShaderPath, const V8_RenderPassDescription& renderPassDesc, const V8_RenderConfig& config = defaultRenderConfig);
    void RemoveRenderer(const std::string& name);
    V8_Renderer* GetRenderer(const std::string& name);
    void Render(const std::string& name);
    void RenderAll();
    void BindScene(const std::string& name, V8_Scene* scene) {
      if (renderers_.find(name) != renderers_.end())
        renderers_[name].BindScene(*scene);
    }

    void UnbindScene(const std::string& name) {
      if (renderers_.find(name) != renderers_.end())
        renderers_[name].UnbindScene();
    }

    void HandleResize();
};
