#include <Core/WindowManager.h>
#include <Core/Logger.h>

Core::WindowManager Core::windowManager = Core::WindowManager();

uint32_t Core::WindowManager::CreateWindow(const Renderer::Context& ctx, const Renderer::Device& device, const char* title, int width, int height, bool resizable) {
  uint32_t id = nextId_;

  windows_[id].window.Init(width, height, title, resizable);
  windows_[id].surface.Init(ctx, id);
  windows_[id].swapchain.Init(device, windows_[id].surface, id);

  nextId_++;

  return id;
}
