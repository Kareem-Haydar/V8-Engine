#include <Core/Application.h>
#include <Renderer/VulkanTypes.h>
#include <Renderer/Config.h>
#include <Renderer/RenderManager.h>

class MyApp : public Core::Application {
  private:
    uint32_t rendererId_ = 0;
    Renderer::Shader vertexShader_;
    Renderer::Shader fragmentShader_;

    void OnInit() override {
      vertexShader_.Init(defaultDevice_, VK_SHADER_STAGE_VERTEX_BIT, "../shaders/vert.spv");
      fragmentShader_.Init(defaultDevice_, VK_SHADER_STAGE_FRAGMENT_BIT, "../shaders/frag.spv");
      rendererId_ = Renderer::renderManager.CreateRenderer(defaultWindow_, defaultSurface_, defaultSwapchain_, vertexShader_, fragmentShader_);
      Renderer::Renderer& rnd = Renderer::renderManager.GetRenderer(rendererId_);
      defaultSwapchain_.GetFramebuffers(rnd.renderPass_);
      V_INFO("initializing");
    }

    void OnFrame(double dt) override {
      Renderer::renderManager.Render(rendererId_);
    }

    void OnShutdown() override {
      V_INFO("shutting down");
      Renderer::renderManager.RemoveRenderer(rendererId_);
    }
};

int main() {
  MyApp app;
  app.Run();
  return 0;
}
