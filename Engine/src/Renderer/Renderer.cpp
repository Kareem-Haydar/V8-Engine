#include <Renderer/Renderer.h>


void Renderer::Renderer::Init(const RendererDescription& info, const Config& config) {
  ctx_ = &info.ctx;
  device_ = &info.device;
  swapchain_ = &info.swapchain;
  surface_ = &info.surface;

  graphicsPass_.Init(info.device, info.swapchain, info.graphicsPassDescription);
  graphicsPipeline_.Init(info.device, info.swapchain, info.vertexShader, info.fragmentShader, graphicsPass_);

  renderPasses_.resize(info.renderPassDescriptions.size());
  for (size_t i = 0; i < info.renderPassDescriptions.size(); i++)
    renderPasses_[i].Init(info.device, info.swapchain, info.renderPassDescriptions[i]);

  imageAvailableSemaphores_.resize(swapchain_->images_.size());
  renderFinishedSemaphores_.resize(swapchain_->images_.size());
  inFlightFences_.resize(swapchain_->images_.size());

  for (size_t i = 0; i < swapchain_->images_.size(); i++) {
    imageAvailableSemaphores_[i].Init(info.device);
    renderFinishedSemaphores_[i].Init(info.device);
    inFlightFences_[i].Init(info.device, true);
  }
}

void Renderer::Renderer::RenderFrame(std::optional<FrameConfig> config) {
  FrameConfig conf = config.value_or(this->config);

  for (size_t i = 0; i < conf.commandBuffers.size(); i++) {
    CommandBuffer& buf = commandBuffers_[i];

    buf.Begin();
  }
}
