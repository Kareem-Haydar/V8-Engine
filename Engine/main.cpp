#include <Core/Application.h>

class MyApp : public Core::Application {
  private:
    uint32_t renderer_;

    void OnInitPre() override {
      V_INFO("initializing");
      config_.appName = "My Vulkan App";
      config_.windowWidth = 800;
      config_.windowHeight = 600;
      config_.resizable = true;
      config_.enableVSync = true;
    }

    void OnInitPost() override {
      renderer_ = renderManager_.CreateRenderer("../shaders/vert.spv", "../shaders/frag.spv", Renderer::RenderPassDescription::Default(context_.swapchainImageFormat_));
    }

    void OnFramePost(double dt) override {
    }

    void OnShutdown() override {
      V_INFO("shutting down");
      renderManager_.Shutdown();
    }
};

int main() {
  MyApp app;
  app.Run();
  return 0;
}
