#include <Renderer/Renderer.h>
#include <Core/Logger.h>

#include <set>

void Renderer::Renderer::Init(const RendererDescription& desc, const Config& config) {
  ctx_ = &desc.ctx;
  device_ = &desc.device;
  swapchain_ = &desc.swapchain;
  surface_ = &desc.surface;

  renderPass_.Init(desc.device, desc.swapchain, std::nullopt, config);

  GraphicsPipelineDescription pipelineDesc = {};
  pipelineDesc.vertexShader = &desc.vertexShader;
  pipelineDesc.fragmentShader = &desc.fragmentShader;
  pipelineDesc.renderPass = &renderPass_;
  pipelineDesc.subpass = 0;

  graphicsPipeline_.Init(desc.device, desc.swapchain, pipelineDesc, config);

  std::set<uint32_t> uniqueQueueFamilies = {
    device_->graphicsQueueFamilyIndex_,
    device_->presentQueueFamilyIndex_
  };

  commandPools_.reserve(uniqueQueueFamilies.size());
  for (auto queueFamily : uniqueQueueFamilies) 
    commandPools_[queueFamily].Init(desc.device, queueFamily);

  commandBuffers_.resize(swapchain_->images_.size());
  for (size_t i = 0; i < commandBuffers_.size(); i++)
    commandBuffers_[i].Allocate(desc.device, commandPools_[device_->graphicsQueueFamilyIndex_], true);

  imageAvailableSemaphores_.resize(swapchain_->images_.size());
  renderFinishedSemaphores_.resize(swapchain_->images_.size());
  inFlightFences_.resize(swapchain_->images_.size());

  for (size_t i = 0; i < swapchain_->images_.size(); i++) {
    imageAvailableSemaphores_[i].Init(desc.device);
    renderFinishedSemaphores_[i].Init(desc.device);
    inFlightFences_[i].Init(desc.device, true);
  }
}

void Renderer::Renderer::RenderFrame() {
  vkWaitForFences(device_->device_, 1, &inFlightFences_[currentFrame_].fence_, VK_TRUE, UINT64_MAX);
  vkResetFences(device_->device_, 1, &inFlightFences_[currentFrame_].fence_);

  uint32_t imageIndex;
  vkAcquireNextImageKHR(device_->device_, swapchain_->swapchain_, UINT64_MAX, imageAvailableSemaphores_[currentFrame_].semaphore_, VK_NULL_HANDLE, &imageIndex);

  vkResetCommandBuffer(commandBuffers_[imageIndex].buffer_, 0);
  commandBuffers_[imageIndex].Begin();

  VkRenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass_.renderPass_;
  renderPassInfo.framebuffer = swapchain_->framebuffers_[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapchain_->extent_;
  renderPassInfo.clearValueCount = 1;
  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffers_[imageIndex].buffer_, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffers_[imageIndex].buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_.pipeline_);

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapchain_->extent_.width);
  viewport.height = static_cast<float>(swapchain_->extent_.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffers_[imageIndex].buffer_, 0, 1, &viewport);

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = swapchain_->extent_;
  vkCmdSetScissor(commandBuffers_[imageIndex].buffer_, 0, 1, &scissor);

  vkCmdDraw(commandBuffers_[imageIndex].buffer_, 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffers_[imageIndex].buffer_);

  commandBuffers_[imageIndex].End();

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores_[currentFrame_].semaphore_};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers_[imageIndex].buffer_;

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores_[currentFrame_].semaphore_};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkQueueSubmit(device_->graphicsQueue_, 1, &submitInfo, inFlightFences_[currentFrame_].fence_);

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain_->swapchain_;
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(device_->presentQueue_, &presentInfo);

  currentFrame_ = (currentFrame_ + 1) % swapchain_->images_.size();
}