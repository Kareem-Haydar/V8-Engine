#include <Renderer/VulkanTypes.h>

#include <Core/Logger.h>

void Renderer::Surface::Init(const Context& ctx, uint32_t windowId) {
  SDL_Window* win = Core::windowManager.GetWindow(windowId).Get();

  if (!SDL_Vulkan_CreateSurface(win, ctx.instance_, &surface_)) 
    V_FATAL("Failed to create SDL Vulkan surface: {}", SDL_GetError());

  instance_ = ctx.instance_;
  windowId_ = windowId;
}

Renderer::Surface::~Surface() {
  if (surface_ != VK_NULL_HANDLE)
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
}
