#pragma once

#include <Core/Context.h>
#include <Renderer/RenderManager.h>

#include <SDL2/SDL.h>

namespace Core {
  class Application {
    protected:
      Core::Config config_ = Core::defaultConfig;
      Core::Context context_;

      Renderer::RenderManager renderManager_;

      virtual void OnInitPre() {}
      virtual void OnInitPost() {}
      virtual void OnRawEvent(SDL_Event& e) {}
      virtual void OnFramePre(double dt) {}
      virtual void OnFramePost(double dt) {}
      virtual void OnShutdown() {}

      virtual void InitDefaultResources() {
      if (SDL_Init(SDL_INIT_VIDEO) != 0) 
        V_FATAL("Failed to initialize SDL video subsystem: {}", SDL_GetError());

        context_.Init(config_);
        renderManager_.Init(&context_);
      }

    public:
      void Run() {
        OnInitPre();
        InitDefaultResources();
        OnInitPost();

        int oldWidth, oldHeight;
        int newWidth, newHeight;

        SDL_GetWindowSize(context_.window_.Get(), &oldWidth, &oldHeight);
        SDL_GetWindowSize(context_.window_.Get(), &newWidth, &newHeight);
        
        bool running = true;
        Uint64 lastTime = SDL_GetPerformanceCounter();
        while (running) {
          Uint64 currentTime = SDL_GetPerformanceCounter();
          double dt = (currentTime - lastTime) / (double)SDL_GetPerformanceFrequency();
          lastTime = currentTime;

          OnFramePre(dt);

          SDL_GetWindowSize(context_.window_.Get(), &newWidth, &newHeight);
          if (newWidth != oldWidth || newHeight != oldHeight) {
            V_DEBUG("Window resized from {}x{} to {}x{}", oldWidth, oldHeight, newWidth, newHeight);

            context_.needsResize_ = true;

            oldWidth = newWidth;
            oldHeight = newHeight;

          }

          if (context_.needsResize_) {
            context_.HandleResize(newWidth, newHeight);
            renderManager_.HandleResize();

            context_.needsResize_ = false;
          } else {
            renderManager_.RenderAll();
          }

          SDL_Event e;
          while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)  running = false;

            OnRawEvent(e);
          }

          OnFramePost(dt);
        }

        OnShutdown();
      }
  };
}
