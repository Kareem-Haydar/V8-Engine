#pragma once

#include <unordered_map>

#include "Window.h"

namespace Core {
  class WindowManager {
    private:
      std::unordered_map<uint32_t, std::unique_ptr<Window>> windows_;
      uint32_t nextId_ = 0;

    public:
      WindowManager() = default;
      ~WindowManager() = default;

      uint32_t CreateWindow(const char* title, int width, int height, bool resizable = false);

      Window& GetWindow(uint32_t id) {
        return *windows_.at(id);
      }

      bool HasWindow(uint32_t id) const {
        return windows_.find(id) != windows_.end();
      }

      void RemoveWindow(uint32_t id) {
        windows_.erase(id);
      }
  };

  extern WindowManager windowManager;
}
