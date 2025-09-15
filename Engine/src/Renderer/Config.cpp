#include <Renderer/Config.h>
#include <Core/Logger.h>

#include <string>

Renderer::Config Renderer::defaultConfig = {
  .polygonMode = Renderer::Config::PolygonMode::Fill,
  .cullMode = Renderer::Config::CullMode::CullModeNone,
  .frontFace = Renderer::Config::FrontFace::Clockwise,
  .depthClampEnable = false,
  .rasterDiscardEnable = false,
  .primitiveRestartEnable = false,
  .lineWidth = 1.0f,
  .sampleCount = Renderer::Config::SampleCount::SampleCountOne,
  .sampleShadingEnable = false,
  .minSampleShading = 0.0f,
  .blendEnable = false,
  .srcColorBlendFactor = Renderer::Config::BlendFactor::BlendFactorOne,
  .dstColorBlendFactor = Renderer::Config::BlendFactor::BlendFactorZero,
  .colorBlendOp = Renderer::Config::BlendOp::BlendOpAdd,
  .srcAlphaBlendFactor = Renderer::Config::BlendFactor::BlendFactorOne,
  .dstAlphaBlendFactor = Renderer::Config::BlendFactor::BlendFactorZero,
  .alphaBlendOp = Renderer::Config::BlendOp::BlendOpAdd,
  .dynamicStates = { Renderer::Config::DynamicState::Viewport, Renderer::Config::DynamicState::Scissor },
  .inputTopology = Renderer::Config::InputTopology::TriangleList,
  .validationLayers = { "VK_LAYER_KHRONOS_validation" },
  .appName = "",
  .appVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
  .engineName = "V8 Engine",
  .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
  .apiVersion = VK_API_VERSION_1_3
};

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
  switch(messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      V_DEBUG("VULKAN: {}", std::string(pCallbackData->pMessage));
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      V_INFO("VULKAN: {}", std::string(pCallbackData->pMessage));
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      V_WARNING("VULKAN: {}", std::string(pCallbackData->pMessage));
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      V_ERROR("VULKAN: {}", std::string(pCallbackData->pMessage));
      break;
    default:
      break;
  }

  return VK_FALSE;
}
