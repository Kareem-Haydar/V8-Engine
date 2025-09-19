#pragma once

#include <Renderer/RenderManager.h>
#include <Core/Context.h>

#include <SDL2/SDL.h>

namespace Core {
  class Application {
    protected:
      Core::Config config_ = Core::defaultConfig;
      Core::Context context_;

      virtual void OnInitPre() {}
      virtual void OnInitPost() {}
      virtual void OnRawEvent(SDL_Event& e) {}
      virtual void OnFrame(double dt) {}
      virtual void OnShutdown() {}

      virtual void InitDefaultResources() {
      if (SDL_Init(SDL_INIT_VIDEO) != 0) 
        V_FATAL("Failed to initialize SDL video subsystem: {}", SDL_GetError());

        context_.Init(config_);
      }

    public:
      void Run() {
        OnInitPre();
        InitDefaultResources();
        OnInitPost();

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
