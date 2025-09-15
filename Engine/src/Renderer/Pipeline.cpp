#include <Renderer/VulkanTypes.h>

#include <Core/Logger.h>

#include <fstream>

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

void Renderer::Shader::Init(const Device& device, VkShaderStageFlagBits type, const char* path) {
  std::vector<char> code = ReadFile(path);

  VkShaderModuleCreateInfo createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  if (vkCreateShaderModule(device.device_, &createInfo, nullptr, &shaderModule_) != VK_SUCCESS) 
    V_FATAL("Failed to create shader module from {}", path);

  stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stageInfo.stage = type;
  stageInfo.module = shaderModule_;
  stageInfo.pName = "main";

  device_ = device.device_;
}

Renderer::Shader::~Shader() {
  if (shaderModule_ != VK_NULL_HANDLE)
    vkDestroyShaderModule(device_, shaderModule_, nullptr);
}

void Renderer::Pipeline::Init(const Device& device, const Swapchain& swapchain, const Shader& vertexShader, const Shader& fragmentShader, const RenderPass& renderPass, const Config& config) {
  VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShader.stageInfo, fragmentShader.stageInfo };

  dynamicStates_.resize(config.dynamicStates.size());

  for (size_t i = 0; i < config.dynamicStates.size(); i++)
    dynamicStates_[i] = static_cast<VkDynamicState>(config.dynamicStates[i]);

  dynamicStateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateInfo_.dynamicStateCount = static_cast<uint32_t>(dynamicStates_.size());
  dynamicStateInfo_.pDynamicStates = dynamicStates_.data();

  vertexInputInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo_.vertexBindingDescriptionCount = 0;
  vertexInputInfo_.pVertexBindingDescriptions = nullptr;
  vertexInputInfo_.vertexAttributeDescriptionCount = 0;
  vertexInputInfo_.pVertexAttributeDescriptions = nullptr;

  inputAssemblyInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyInfo_.topology = static_cast<VkPrimitiveTopology>(config.inputTopology);
  inputAssemblyInfo_.primitiveRestartEnable = static_cast<VkBool32>(config.primitiveRestartEnable);

  viewportInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo_.viewportCount = 1;
  viewportInfo_.scissorCount = 1;

  rasterizerInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerInfo_.depthClampEnable = static_cast<VkBool32>(config.depthClampEnable);
  rasterizerInfo_.rasterizerDiscardEnable = static_cast<VkBool32>(config.rasterDiscardEnable);
  rasterizerInfo_.polygonMode = static_cast<VkPolygonMode>(config.polygonMode);
  rasterizerInfo_.cullMode = static_cast<VkCullModeFlagBits>(config.cullMode);
  rasterizerInfo_.frontFace = static_cast<VkFrontFace>(config.frontFace);
  rasterizerInfo_.depthBiasEnable = VK_FALSE;
  rasterizerInfo_.depthBiasConstantFactor = 0.0f;
  rasterizerInfo_.depthBiasClamp = 0.0f;
  rasterizerInfo_.depthBiasSlopeFactor = 0.0f;

  multisampleInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleInfo_.sampleShadingEnable = static_cast<VkBool32>(config.sampleShadingEnable);
  multisampleInfo_.rasterizationSamples = static_cast<VkSampleCountFlagBits>(config.sampleCount);
  multisampleInfo_.minSampleShading = 1.0f;
  multisampleInfo_.pSampleMask = nullptr;
  multisampleInfo_.alphaToCoverageEnable = VK_FALSE;
  multisampleInfo_.alphaToOneEnable = VK_FALSE;

  colorBlendAttachment_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment_.blendEnable = static_cast<VkBool32>(config.blendEnable);
  colorBlendAttachment_.srcColorBlendFactor = static_cast<VkBlendFactor>(config.srcColorBlendFactor);
  colorBlendAttachment_.dstColorBlendFactor = static_cast<VkBlendFactor>(config.dstColorBlendFactor);
  colorBlendAttachment_.colorBlendOp = static_cast<VkBlendOp>(config.colorBlendOp);
  colorBlendAttachment_.srcAlphaBlendFactor = static_cast<VkBlendFactor>(config.srcAlphaBlendFactor);
  colorBlendAttachment_.dstAlphaBlendFactor = static_cast<VkBlendFactor>(config.dstAlphaBlendFactor);
  colorBlendAttachment_.alphaBlendOp = static_cast<VkBlendOp>(config.alphaBlendOp);

  colorBlendingInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendingInfo_.logicOpEnable = VK_FALSE;
  colorBlendingInfo_.logicOp = VK_LOGIC_OP_COPY;
  colorBlendingInfo_.attachmentCount = 1;
  colorBlendingInfo_.pAttachments = &colorBlendAttachment_;
  colorBlendingInfo_.blendConstants[0] = 0.0f;
  colorBlendingInfo_.blendConstants[1] = 0.0f;
  colorBlendingInfo_.blendConstants[2] = 0.0f;
  colorBlendingInfo_.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo layoutInfo {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layoutInfo.setLayoutCount = 0;
  layoutInfo.pSetLayouts = nullptr;
  layoutInfo.pushConstantRangeCount = 0;
  layoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(device.device_, &layoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS)
    V_FATAL("Failed to create pipeline layout");

  VkGraphicsPipelineCreateInfo pipelineInfo {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo_;
  pipelineInfo.pInputAssemblyState = &inputAssemblyInfo_;
  pipelineInfo.pViewportState = &viewportInfo_;
  pipelineInfo.pRasterizationState = &rasterizerInfo_;
  pipelineInfo.pMultisampleState = &multisampleInfo_;
  pipelineInfo.pColorBlendState = &colorBlendingInfo_;
  pipelineInfo.layout = pipelineLayout_;
  pipelineInfo.renderPass = renderPass.renderPass_;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.pDynamicState = &dynamicStateInfo_;

  if (vkCreateGraphicsPipelines(device.device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline_) != VK_SUCCESS)
    V_FATAL("Failed to create graphics pipeline");

  device_ = device.device_;
}

Renderer::Pipeline::~Pipeline() {
  if (pipelineLayout_ != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);

  if (pipeline_ != VK_NULL_HANDLE)
    vkDestroyPipeline(device_, pipeline_, nullptr);
}
