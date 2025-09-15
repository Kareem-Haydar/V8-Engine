#include <Core/Application.h>
#include <Renderer/VulkanTypes.h>
#include <Renderer/Config.h>

class MyApp : public Core::Application {
  public: 
    Renderer::Context ctx;
    Renderer::Surface surface;
    Renderer::Device device;
    Renderer::Swapchain swapchain;
    Renderer::RenderPass renderPass;
    Renderer::Shader vertexShader;
    Renderer::Shader fragmentShader;
    Renderer::Pipeline pipeline;

    void OnInit() override {
      V_INFO("initializing");

      ctx.Init(defaultWindow_);
      surface.Init(ctx, defaultWindow_);
      device.Init(ctx, surface);
      swapchain.Init(device, surface, defaultWindow_);
      renderPass.Init(device, swapchain);

      vertexShader.Init(device, VK_SHADER_STAGE_VERTEX_BIT, "../shaders/vert.spv");
      fragmentShader.Init(device, VK_SHADER_STAGE_FRAGMENT_BIT, "../shaders/frag.spv");
      pipeline.Init(device, swapchain, vertexShader, fragmentShader, renderPass);
      swapchain.GetFramebuffers(renderPass);
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
