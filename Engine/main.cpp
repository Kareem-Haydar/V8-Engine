#include <Core/Application.h>
#include <Scene/Types.h>

class MyApp : public V8_Application {
  private:
    V8_Scene scene_;
    V8_SceneManager sceneManager_;

    void OnInitPre() override {
      V_INFO("initializing");
      config_.appName = "My Vulkan App";
      config_.windowWidth = 800;
      config_.windowHeight = 600;
      config_.resizable = true;
      config_.enableVSync = true;
    }

    void OnInitPost() override {
      renderManager_.CreateRenderer("default", "../shaders/vert.spv", "../shaders/frag.spv", V8_RenderPassDescription::Default(context_.swapchainImageFormat_));

      std::shared_ptr<V8_StaticMesh> mesh = std::make_shared<V8_StaticMesh>();
      std::vector<V8_Vertex> vertices = {
        { .position = {0.5f, 0.5f, 0.0f}, .normal = {0.0f, 0.0f, 0.0f}, .color = {1.0f, 0.0f, 0.0f}, .uv = {0.0f, 0.0f} },
        { .position = {-0.5f, 0.5f, 0.0f}, .normal = {0.0f, 0.0f, 0.0f}, .color = {0.0f, 1.0f, 0.0f}, .uv = {1.0f, 0.0f} },
        { .position = {-0.5f, -0.5f, 0.0f}, .normal = {0.0f, 0.0f, 0.0f}, .color = {0.0f, 0.0f, 1.0f}, .uv = {0.0f, 1.0f} },
        { .position = {0.5f, -0.5f, 0.0f}, .normal = {0.0f, 0.0f, 0.0f}, .color = {0.0f, 0.0f, 1.0f}, .uv = {0.0f, 1.0f} }
      };

      std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

      mesh->Init(context_, vertices, indices);

      sceneManager_.AddScene("main");
      V8_Entity entity = sceneManager_.AddEntity("main");
      sceneManager_.AddComponent<V8_StaticMesh>("main", entity, mesh);

      renderManager_.BindScene("default", &sceneManager_.GetScene("main"));
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
