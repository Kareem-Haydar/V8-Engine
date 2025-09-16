#pragma once

#include <unordered_map>
#include <tuple>

#include <Renderer/VulkanTypes.h>
#include <Core/Window.h>

namespace Core {
  struct WindowEntry {
    Window window;
    Renderer::Surface surface;
    Renderer::Swapchain swapchain;
  };

  class WindowManager {
    private:
      std::unordered_map<uint32_t, WindowEntry> windows_;
      uint32_t nextId_ = 0;

    public:
      WindowManager() = default;
      ~WindowManager() = default;

      uint32_t CreateWindow(const Renderer::Context& ctx, const Renderer::Device& device, const char* title, int width, int height, bool resizable = false);

      WindowEntry& GetWindow(uint32_t id) {
        return windows_.at(id);
      }

      bool Exists(uint32_t id) const {
        return windows_.find(id) != windows_.end();
      }

      void RemoveWindow(uint32_t id) {
        windows_.erase(id);
      }
  };

  extern WindowManager windowManager;
}
