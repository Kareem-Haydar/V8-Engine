#include <Renderer/Renderer.h>
#include <Scene/Types.h>

#include <fstream>
#include <vector>

std::vector<char> ReadFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    V_ERROR("failed to open file {}", filename);
    return {};
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

void V8_Renderer::V8_Renderer::Init(V8_Context& ctx, const char* vertexShaderPath, const char* fragmentShaderPath, const std::optional<V8_RenderPassDescription>& renderPassDesc, const V8_RenderConfig& config) {
  context_ = &ctx;

  V8_RenderPassDescription desc = renderPassDesc.value_or(V8_RenderPassDescription::Default(context_->swapchainImageFormat_, config));

  VkRenderPassCreateInfo renderPassInfo {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(desc.attachments_.size());
  renderPassInfo.pAttachments = desc.attachments_.data();
  renderPassInfo.subpassCount = static_cast<uint32_t>(desc.subpasses_.size());
  renderPassInfo.pSubpasses = desc.subpasses_.data();
  renderPassInfo.dependencyCount = static_cast<uint32_t>(desc.dependencies_.size());
  renderPassInfo.pDependencies = desc.dependencies_.data();

  if (vkCreateRenderPass(context_->device_, &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS)
    V_FATAL("Failed to create render pass");

  VkDescriptorSetLayoutBinding descriptorBinding {};
  descriptorBinding.binding = 0;
  descriptorBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorBinding.descriptorCount = 1;
  descriptorBinding.stageFlags = static_cast<VkShaderStageFlagBits>(config.descriptorStage);
  descriptorBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo {};
  descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorLayoutInfo.bindingCount = 1;
  descriptorLayoutInfo.pBindings = &descriptorBinding;

  VK_CHECK(vkCreateDescriptorSetLayout(context_->device_, &descriptorLayoutInfo, nullptr, &descriptorSetLayout_));

  VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  VK_CHECK(vkCreatePipelineLayout(context_->device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_));

  std::vector<char> vertShaderCode = ReadFile(vertexShaderPath);
  std::vector<char> fragShaderCode = ReadFile(fragmentShaderPath);

  VkShaderModuleCreateInfo vertShaderModuleInfo {};
  vertShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vertShaderModuleInfo.codeSize = vertShaderCode.size();
  vertShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

  VkShaderModuleCreateInfo fragShaderModuleInfo {};
  fragShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  fragShaderModuleInfo.codeSize = fragShaderCode.size();
  fragShaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;

  if (vkCreateShaderModule(context_->device_, &vertShaderModuleInfo, nullptr, &vertShaderModule) != VK_SUCCESS)
    V_FATAL("Failed to create vertex shader module");

  if (vkCreateShaderModule(context_->device_, &fragShaderModuleInfo, nullptr, &fragShaderModule) != VK_SUCCESS)
    V_FATAL("Failed to create fragment shader module");

  VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

  auto vertexBindingDescription = V8_Vertex::GetBindingDescription();
  auto vertexAttributeDescriptions = V8_Vertex::GetAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDescription;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo {};
  inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyInfo.topology = static_cast<VkPrimitiveTopology>(config.inputTopology);
  inputAssemblyInfo.primitiveRestartEnable = static_cast<VkBool32>(config.primitiveRestartEnable);

  std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamicStateInfo {};
  dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicStateInfo.pDynamicStates = dynamicStates.data();

  VkPipelineViewportStateCreateInfo viewportInfo {};
  viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo.viewportCount = 1;
  viewportInfo.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizerInfo {};
  rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerInfo.depthClampEnable = VK_FALSE;
  rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizerInfo.lineWidth = 1.0f;
  rasterizerInfo.cullMode = static_cast<VkCullModeFlagBits>(config.cullMode);
  rasterizerInfo.frontFace = static_cast<VkFrontFace>(config.frontFace);
  rasterizerInfo.depthBiasEnable = VK_FALSE;
  rasterizerInfo.depthBiasConstantFactor = 0.0f;
  rasterizerInfo.depthBiasClamp = 0.0f;
  rasterizerInfo.depthBiasSlopeFactor = 0.0f;

  VkPipelineMultisampleStateCreateInfo multisampleInfo {};
  multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleInfo.sampleShadingEnable = VK_FALSE;
  multisampleInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(config.sampleCount);
  multisampleInfo.minSampleShading = 0.0f;
  multisampleInfo.pSampleMask = nullptr;
  multisampleInfo.alphaToCoverageEnable = VK_FALSE;
  multisampleInfo.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = static_cast<VkBool32>(config.blendEnable);
  colorBlendAttachment.srcColorBlendFactor = static_cast<VkBlendFactor>(config.srcColorBlendFactor);
  colorBlendAttachment.dstColorBlendFactor = static_cast<VkBlendFactor>(config.dstColorBlendFactor);
  colorBlendAttachment.colorBlendOp = static_cast<VkBlendOp>(config.colorBlendOp);
  colorBlendAttachment.srcAlphaBlendFactor = static_cast<VkBlendFactor>(config.srcAlphaBlendFactor);
  colorBlendAttachment.dstAlphaBlendFactor = static_cast<VkBlendFactor>(config.dstAlphaBlendFactor);
  colorBlendAttachment.alphaBlendOp = static_cast<VkBlendOp>(config.alphaBlendOp);

  VkPipelineColorBlendStateCreateInfo colorBlendingInfo {};
  colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendingInfo.logicOpEnable = VK_FALSE;
  colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendingInfo.attachmentCount = 1;
  colorBlendingInfo.pAttachments = &colorBlendAttachment;
  colorBlendingInfo.blendConstants[0] = 0.0f;
  colorBlendingInfo.blendConstants[1] = 0.0f;
  colorBlendingInfo.blendConstants[2] = 0.0f;
  colorBlendingInfo.blendConstants[3] = 0.0f;

  VkPipelineDepthStencilStateCreateInfo depthStencilInfo {};
  depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilInfo.depthTestEnable = VK_TRUE;
  depthStencilInfo.depthWriteEnable = VK_TRUE;
  depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  depthStencilInfo.stencilTestEnable = VK_FALSE;

  VkGraphicsPipelineCreateInfo pipelineInfo {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
  pipelineInfo.pViewportState = &viewportInfo;
  pipelineInfo.pRasterizationState = &rasterizerInfo;
  pipelineInfo.pMultisampleState = &multisampleInfo;
  pipelineInfo.pColorBlendState = &colorBlendingInfo;
  pipelineInfo.pDepthStencilState = &depthStencilInfo;
  pipelineInfo.layout = pipelineLayout_;
  pipelineInfo.renderPass = renderPass_;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.pDynamicState = &dynamicStateInfo;

  VK_CHECK(vkCreateGraphicsPipelines(context_->device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline_));

  vkDestroyShaderModule(context_->device_, vertShaderModule, nullptr);
  vkDestroyShaderModule(context_->device_, fragShaderModule, nullptr);

  framebuffers_.resize(context_->swapchainImages_.size());
  for (size_t i = 0; i < framebuffers_.size(); i++) {
    VkFramebufferCreateInfo framebufferInfo {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass_;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &context_->swapchainImageViews_[i];
    framebufferInfo.width = context_->swapchainExtent_.width;
    framebufferInfo.height = context_->swapchainExtent_.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(context_->device_, &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS)
      V_FATAL("Failed to create framebuffer");
  }

  commandBuffers_.resize(context_->swapchainImages_.size());
  for (size_t i = 0; i < commandBuffers_.size(); i++) {
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = context_->commandPools_[context_->graphicsQueueFamilyIndex_];
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(context_->device_, &allocInfo, &commandBuffers_[i]) != VK_SUCCESS)
      V_FATAL("Failed to allocate command buffers");
  }
}

V8_Renderer::V8_Renderer::~V8_Renderer() {
  vkDeviceWaitIdle(context_->device_);

  vkDestroyDescriptorSetLayout(context_->device_, descriptorSetLayout_, nullptr);

  for (auto framebuffer : framebuffers_)
    vkDestroyFramebuffer(context_->device_, framebuffer, nullptr);

  if (pipelineLayout_ != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(context_->device_, pipelineLayout_, nullptr);

  if (pipeline_ != VK_NULL_HANDLE)
    vkDestroyPipeline(context_->device_, pipeline_, nullptr);

  if (renderPass_ != VK_NULL_HANDLE)
    vkDestroyRenderPass(context_->device_, renderPass_, nullptr);

  scene_ = nullptr;
}

void V8_Renderer::V8_Renderer::Render() {
  if (scene_ == nullptr) {
    V_WARNING("No scene bound to renderer");
    return;
  }

  if (context_->needsResize_) {
    V_DEBUG("Swapchain out of date");
    vkDeviceWaitIdle(context_->device_);
    return;
  }

  vkWaitForFences(context_->device_, 1, &context_->inFlightFences_[currentFrame_], VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult res = vkAcquireNextImageKHR(context_->device_, context_->swapchain_, UINT64_MAX, context_->imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex);

  if (res == VK_ERROR_OUT_OF_DATE_KHR) {
    V_DEBUG("Swapchain out of date");
    context_->needsResize_ = true;
    return;
  } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
    V_FATAL("Failed to acquire swapchain image");
  }

  vkResetFences(context_->device_, 1, &context_->inFlightFences_[currentFrame_]);
  vkResetCommandBuffer(commandBuffers_[currentFrame_], 0);

  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffers_[currentFrame_], &beginInfo) != VK_SUCCESS)
    V_FATAL("Failed to begin recording command buffer");

  VkRenderPassBeginInfo renderPassInfo {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass_;
  renderPassInfo.framebuffer = framebuffers_[imageIndex];
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = context_->swapchainExtent_;

  VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffers_[currentFrame_], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffers_[currentFrame_], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

  VkViewport viewport {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(context_->swapchainExtent_.width);
  viewport.height = static_cast<float>(context_->swapchainExtent_.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffers_[currentFrame_], 0, 1, &viewport);

  VkRect2D scissor {};
  scissor.offset = { 0, 0 };
  scissor.extent = context_->swapchainExtent_;
  vkCmdSetScissor(commandBuffers_[currentFrame_], 0, 1, &scissor);

  for (auto& e : scene_->registry.entities_) {
    if (scene_->registry.GetComponent<V8_StaticMesh>(e) == nullptr)
      continue;

    V8_StaticMesh* mesh = scene_->registry.GetComponent<V8_StaticMesh>(e);

    VkBuffer vertexBuffers[] = { mesh->vertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    VkBuffer indexBuffers[] = { mesh->indexBuffer };

    vkCmdBindVertexBuffers(commandBuffers_[currentFrame_], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffers_[currentFrame_], mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffers_[currentFrame_], static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
  }

  vkCmdEndRenderPass(commandBuffers_[currentFrame_]);

  if (vkEndCommandBuffer(commandBuffers_[currentFrame_]) != VK_SUCCESS)
    V_FATAL("Failed to record command buffer");

  VkSubmitInfo submitInfo {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];

  VkSemaphore waitSemaphores[] = { context_->imageAvailableSemaphores_[currentFrame_] };
  VkSemaphore signalSemaphores[] = { context_->renderFinishedSemaphores_[imageIndex] };
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  if (vkQueueSubmit(context_->graphicsQueue_, 1, &submitInfo, context_->inFlightFences_[currentFrame_]) != VK_SUCCESS)
    V_FATAL("Failed to submit draw command buffer");

  VkPresentInfoKHR presentInfo {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &context_->swapchain_;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  res = vkQueuePresentKHR(context_->presentQueue_, &presentInfo);
  if (res == VK_ERROR_OUT_OF_DATE_KHR) {
    V_DEBUG("Swapchain out of date (failed to present swapchain image)");
    context_->needsResize_ = true;
    return;
  } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
    V_FATAL("Failed to present swapchain image");
  }

  currentFrame_ = (currentFrame_ + 1) % context_->swapchainImages_.size();
}

void V8_Renderer::V8_Renderer::HandleResize() {
  for (auto& fb : framebuffers_)
    vkDestroyFramebuffer(context_->device_, fb, nullptr);

  framebuffers_.resize(context_->swapchainImages_.size());
  for (size_t i = 0; i < framebuffers_.size(); i++) {
    VkFramebufferCreateInfo framebufferInfo {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass_;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &context_->swapchainImageViews_[i];
    framebufferInfo.width = context_->swapchainExtent_.width;
    framebufferInfo.height = context_->swapchainExtent_.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(context_->device_, &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS)
      V_FATAL("Failed to create framebuffer");
  }

  currentFrame_ = 0;
}
