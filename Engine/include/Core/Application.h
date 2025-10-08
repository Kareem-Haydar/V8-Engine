#pragma once

#include <Renderer/RenderManager.h>
#include <Core/Context.h>

#include <SDL2/SDL.h>

class V8_Application {
  protected:
    V8_CoreConfig config_ = defaultConfig;
    V8_Context context_;

    V8_RenderManager renderManager_;

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

        if (context_.needsResize_) {
          V_DEBUG("Window resized");

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
