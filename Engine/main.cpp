#include <Core/Application.h>
#include <Renderer/VulkanTypes.h>
#include <Renderer/Config.h>

class MyApp : public Core::Application {
  public: 
    void OnInit() override {
      V_INFO("initializing");
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
