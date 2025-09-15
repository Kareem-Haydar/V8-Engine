#include <Renderer/Renderer.h>


void Renderer::Renderer::Init(const RendererDescription& info, const Config& config) {
  ctx_ = &info.ctx;
  device_ = &info.device;
  swapchain_ = &info.swapchain;
  surface_ = &info.surface;

  renderPasses_.resize(info.renderPassDescriptions.size());
  for (size_t i = 0; i < info.renderPassDescriptions.size(); i++)
    renderPasses_[i].Init(info.device, info.swapchain, info.renderPassDescriptions[i]);
}

void Renderer::Renderer::RenderFrame(std::optional<FrameConfig> config) {
  FrameConfig conf = config.value_or(this->config);

  for (size_t i = 0; i < conf.commandBuffers.size(); i++) {
    CommandBuffer& buf = commandBuffers_[i];

    buf.Begin();
  }
}
