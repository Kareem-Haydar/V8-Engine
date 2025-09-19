#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "Logger.h"

namespace Core {
  class Window {
    private:
      SDL_Window* window_;

    public:
      void Init(uint32_t width, uint32_t height, const char* title, bool resizable = false, bool fullscreen = false) {
        int flags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN;
        if (resizable) flags |= SDL_WINDOW_RESIZABLE;
        if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;

        window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

        if (!window_) V_FATAL("Failed to create SDL window: {}", SDL_GetError());
      }

      ~Window() {
        if (window_)
          SDL_DestroyWindow(window_);
      }

      SDL_Window* Get() {
        return window_;
      }

      glm::uvec2 GetSize() {
        if (!window_) {
          V_ERROR("Invalid window");
          return {0, 0};
        }

        int w, h;
        SDL_GetWindowSize(window_, &w, &h);
        return {w, h};
      }

      void SetSize(glm::uvec2 size) {
        if (!window_) {
          V_ERROR("Invalid window");
          return;
        }

        SDL_SetWindowSize(window_, size.x, size.y);
      }

      const char* GetTitle() {
        if (!window_) {
          V_ERROR("Invalid window");
          return "";
        }

        return SDL_GetWindowTitle(window_);
      }

      void SetTitle(const char* title) {
        if (!window_) {
          V_ERROR("Invalid window");
          return;
        }

        SDL_SetWindowTitle(window_, title);
      }
  };
}
