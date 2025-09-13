#include <Core/Application.h>
#include <Renderer/Vulkan/VulkanTypes.h>
#include <Renderer/Vulkan/Config.h>

class MyApp : public Core::Application {
  public: 
    Renderer::Vulkan::Context ctx;
    Renderer::Vulkan::Surface surface;
    Renderer::Vulkan::Device device;

    void OnInit() override {
      V_INFO("initializing");

      ctx.Init("V8-Engine", 1, "Engine", 1, VK_API_VERSION_1_3, defaultWindow_);
      surface.Init(ctx, defaultWindow_);
      device.Init(ctx, surface, Renderer::Vulkan::Config::CHOOSE_BEST_DEVICE);
    }

    void OnShutdown() override {
      V_INFO("shutting down");
    }

    void OnRawEvent(SDL_Event& e) override {
      V_DEBUG("raw event: {}", e.type);
    }
};

int main() {
  MyApp app;
  app.Run();
  return 0;
}
