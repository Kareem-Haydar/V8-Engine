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

void Renderer::Pipeline::Init(const Device& device, const Swapchain& swapchain, const GraphicsPipelineDescription& description, const Config& config) {
VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
VkPipelineViewportStateCreateInfo viewportInfo = {};
VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
VkPipelineColorBlendStateCreateInfo colorBlendingInfo = {};
VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};

  VkPipelineShaderStageCreateInfo shaderStages[] = { description.vertexShader->stageInfo, description.fragmentShader->stageInfo };

  dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(description.dynamicStates.size());
  dynamicStateInfo.pDynamicStates = description.dynamicStates.data();

  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;

  inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyInfo.topology = static_cast<VkPrimitiveTopology>(config.inputTopology);
  inputAssemblyInfo.primitiveRestartEnable = static_cast<VkBool32>(config.primitiveRestartEnable);

  viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo.viewportCount = 1;
  viewportInfo.scissorCount = 1;

  rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerInfo.depthClampEnable = static_cast<VkBool32>(config.depthClampEnable);
  rasterizerInfo.rasterizerDiscardEnable = static_cast<VkBool32>(config.rasterDiscardEnable);
  rasterizerInfo.polygonMode = static_cast<VkPolygonMode>(config.polygonMode);
  rasterizerInfo.cullMode = static_cast<VkCullModeFlagBits>(config.cullMode);
  rasterizerInfo.frontFace = static_cast<VkFrontFace>(config.frontFace);
  rasterizerInfo.depthBiasEnable = VK_FALSE;
  rasterizerInfo.depthBiasConstantFactor = 0.0f;
  rasterizerInfo.depthBiasClamp = 0.0f;
  rasterizerInfo.depthBiasSlopeFactor = 0.0f;

  multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleInfo.sampleShadingEnable = static_cast<VkBool32>(config.sampleShadingEnable);
  multisampleInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(config.sampleCount);
  multisampleInfo.minSampleShading = 1.0f;
  multisampleInfo.pSampleMask = nullptr;
  multisampleInfo.alphaToCoverageEnable = VK_FALSE;
  multisampleInfo.alphaToOneEnable = VK_FALSE;

  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = static_cast<VkBool32>(config.blendEnable);
  colorBlendAttachment.srcColorBlendFactor = static_cast<VkBlendFactor>(config.srcColorBlendFactor);
  colorBlendAttachment.dstColorBlendFactor = static_cast<VkBlendFactor>(config.dstColorBlendFactor);
  colorBlendAttachment.colorBlendOp = static_cast<VkBlendOp>(config.colorBlendOp);
  colorBlendAttachment.srcAlphaBlendFactor = static_cast<VkBlendFactor>(config.srcAlphaBlendFactor);
  colorBlendAttachment.dstAlphaBlendFactor = static_cast<VkBlendFactor>(config.dstAlphaBlendFactor);
  colorBlendAttachment.alphaBlendOp = static_cast<VkBlendOp>(config.alphaBlendOp);

  colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendingInfo.logicOpEnable = VK_FALSE;
  colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendingInfo.attachmentCount = 1;
  colorBlendingInfo.pAttachments = &colorBlendAttachment;
  colorBlendingInfo.blendConstants[0] = 0.0f;
  colorBlendingInfo.blendConstants[1] = 0.0f;
  colorBlendingInfo.blendConstants[2] = 0.0f;
  colorBlendingInfo.blendConstants[3] = 0.0f;

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
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
  pipelineInfo.pViewportState = &viewportInfo;
  pipelineInfo.pRasterizationState = &rasterizerInfo;
  pipelineInfo.pMultisampleState = &multisampleInfo;
  pipelineInfo.pColorBlendState = &colorBlendingInfo;
  pipelineInfo.layout = pipelineLayout_;
  pipelineInfo.renderPass = description.renderPass->renderPass_;
  pipelineInfo.subpass = description.subpass;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.pDynamicState = &dynamicStateInfo;

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
