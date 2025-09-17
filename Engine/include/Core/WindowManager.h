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
      std::unordered_map<uint32_t, Window> windows_;
      uint32_t nextId_ = 0;

    public:
      WindowManager() = default;
      ~WindowManager() = default;

      uint32_t CreateWindow(const char* title, int width, int height, bool resizable = false);

      Window& GetWindow(uint32_t id) {
        auto it = windows_.find(id);
        if (it != windows_.end())
          return it->second;

        V_ERROR("Window with ID {} does not exist.", id);
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
