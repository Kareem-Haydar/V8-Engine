#include <Renderer/Config.h>
#include <Core/Logger.h>

#include <string>

V8_RenderConfig defaultRenderConfig = {
  .polygonMode = V8_RenderConfig::PolygonMode::Fill,
  .cullMode = V8_RenderConfig::CullMode::CullModeNone,
  .frontFace = V8_RenderConfig::FrontFace::Clockwise,
  .depthClampEnable = false,
  .rasterDiscardEnable = false,
  .primitiveRestartEnable = false,
  .lineWidth = 1.0f,
  .sampleCount = V8_RenderConfig::SampleCount::SampleCountOne,
  .sampleShadingEnable = false,
  .minSampleShading = 0.0f,
  .blendEnable = false,
  .srcColorBlendFactor = V8_RenderConfig::BlendFactor::BlendFactorOne,
  .dstColorBlendFactor = V8_RenderConfig::BlendFactor::BlendFactorZero,
  .colorBlendOp = V8_RenderConfig::BlendOp::BlendOpAdd,
  .srcAlphaBlendFactor = V8_RenderConfig::BlendFactor::BlendFactorOne,
  .dstAlphaBlendFactor = V8_RenderConfig::BlendFactor::BlendFactorZero,
  .alphaBlendOp = V8_RenderConfig::BlendOp::BlendOpAdd,
  .dynamicStates = { V8_RenderConfig::DynamicState::Viewport, V8_RenderConfig::DynamicState::Scissor },
  .inputTopology = V8_RenderConfig::InputTopology::TriangleList,
  .validationLayers = { "VK_LAYER_KHRONOS_validation" },
  .appName = "",
  .appVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
  .engineName = "V8 Engine",
  .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
  .apiVersion = VK_API_VERSION_1_3
};

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
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
