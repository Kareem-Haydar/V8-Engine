#include <Core/Application.h>
#include <Renderer/VulkanTypes.h>
#include <Renderer/Config.h>

class MyApp : public Core::Application {
  public: 
    Renderer::Context ctx;
    Renderer::Surface surface;
    Renderer::Device device;

    void OnInit() override {
      V_INFO("initializing");

      ctx.Init("V8-Engine", 1, "Engine", 1, VK_API_VERSION_1_3, defaultWindow_);
      surface.Init(ctx, defaultWindow_);
      device.Init(ctx, surface, Renderer::Config::CHOOSE_BEST_DEVICE);
    }

    void OnShutdown() override {
      V_INFO("shutting down");
    }
};

int main() {
  MyApp app;
  app.Run();
  return 0;
}
