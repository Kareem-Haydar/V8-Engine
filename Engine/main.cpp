#include <Core/Application.h>
#include <Renderer/VulkanTypes.h>
#include <Renderer/Config.h>
#include <Renderer/RenderManager.h>

class MyApp : public Core::Application {
  private:
    void OnInitPre() override {
      V_INFO("initializing");
      config_.appName = "My Vulkan App";
      config_.windowWidth = 800;
      config_.windowHeight = 600;
      config_.resizable = false;
      config_.enableVSync = true;
    }

    void OnFrame(double dt) override {
      
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
