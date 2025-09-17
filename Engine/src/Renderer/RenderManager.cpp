#include <Renderer/RenderManager.h>

namespace Renderer {
  RenderManager renderManager;
}

uint32_t Renderer::RenderManager::CreateRenderer(uint32_t windowId, const Surface& surface, const Swapchain& swapchain, const Shader& vertexShader, const Shader& fragmentShader, const Config& config) {
  uint32_t rendererId = nextRendererId_;

  if (!ctx_ || !device_)
    V_ERROR("RenderManager not initialized with context and device");

  if (renderers_.find(windowId) != renderers_.end())
    V_ERROR("Renderer for this window ID already exists.");

  Core::Window& windowEntry = Core::windowManager.GetWindow(windowId);

  RendererDescription desc = {
    *ctx_,
    *device_,
    surface,
    swapchain,
    vertexShader,
    fragmentShader
  };

  nextRendererId_++;
  renderers_[rendererId].Init(desc, config);
  return rendererId;
}