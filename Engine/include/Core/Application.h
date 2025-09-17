#pragma once

#include <SDL2/SDL.h>

#include "WindowManager.h"
#include "Logger.h"

namespace Core {
  class Application {
    protected:
      Renderer::Context defaultContext_;
      Renderer::Device defaultDevice_;
      Renderer::Surface defaultSurface_;
      Renderer::Swapchain defaultSwapchain_;
      uint32_t defaultWindow_;

      virtual void OnInit() {}
      virtual void OnRawEvent(SDL_Event& e) {}
      virtual void OnFrame(double dt) {}
      virtual void OnShutdown() {}

      virtual void InitDefaultResources() {
        defaultWindow_ = Core::windowManager.CreateWindow("V8 Engine", 1280, 720);
        defaultContext_.Init(defaultWindow_);
        defaultSurface_.Init(defaultContext_, defaultWindow_);
        defaultDevice_.Init(defaultContext_, defaultSurface_);
        defaultSwapchain_.Init(defaultDevice_, defaultSurface_, defaultWindow_);
      }

    public:
      void Run() {
        // initlization
        if (SDL_Init(SDL_INIT_VIDEO) != 0) 
          V_FATAL("Failed to initialize SDL video subsystem: {}", SDL_GetError());

        OnInit();
        InitDefaultResources();

        // main loop

        bool running = true;
        Uint64 lastTime = SDL_GetPerformanceCounter();
        while (running) {
          Uint64 currentTime = SDL_GetPerformanceCounter();
          double dt = (currentTime - lastTime) / (double)SDL_GetPerformanceFrequency();
          lastTime = currentTime;
          OnFrame(dt);

          SDL_Event e;
          while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)  running = false;

            OnRawEvent(e);
          }
        }

        OnShutdown();
      }
  };
}
