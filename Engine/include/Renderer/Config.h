#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#ifdef NDEBUG
  #define DEBUG_MODE false
#else
  #define DEBUG_MODE true
#endif

struct V8_RenderConfig {
  enum class ShaderType {
    Vertex   = VK_SHADER_STAGE_VERTEX_BIT,
    Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    Compute  = VK_SHADER_STAGE_COMPUTE_BIT
  };

  enum class InputTopology {
    PointList     = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    LineList      = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    LineStrip     = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    TriangleList  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    TriangleStrip = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
  };

  enum class PolygonMode {
    Fill  = VK_POLYGON_MODE_FILL,
    Line  = VK_POLYGON_MODE_LINE,
    Point = VK_POLYGON_MODE_POINT
  };

  enum class CullMode {
    CullModeNone  = VK_CULL_MODE_NONE,
    CullModeBack  = VK_CULL_MODE_BACK_BIT,
    CullModeFront = VK_CULL_MODE_FRONT_BIT
  };

  enum class FrontFace {
    Clockwise = VK_FRONT_FACE_CLOCKWISE,
    CounterClockwise = VK_FRONT_FACE_COUNTER_CLOCKWISE
  };

  enum class SampleCount {
    SampleCountOne       = VK_SAMPLE_COUNT_1_BIT,
    SampleCountTwo       = VK_SAMPLE_COUNT_2_BIT,
    SampleCountFour      = VK_SAMPLE_COUNT_4_BIT,
    SampleCountEight     = VK_SAMPLE_COUNT_8_BIT,
    SampleCountSixteen   = VK_SAMPLE_COUNT_16_BIT,
    SampleCountThirtyTwo = VK_SAMPLE_COUNT_32_BIT,
    SampleCountSixtyFour = VK_SAMPLE_COUNT_64_BIT
  };

  enum class DynamicState {
    Viewport = VK_DYNAMIC_STATE_VIEWPORT,
    Scissor  = VK_DYNAMIC_STATE_SCISSOR
  };

  enum class BlendFactor {
    BlendFactorZero = VK_BLEND_FACTOR_ZERO,
    BlendFactorOne  = VK_BLEND_FACTOR_ONE
  };

  enum class BlendOp {
    BlendOpAdd      = VK_BLEND_OP_ADD,
    BlendOpSubtract = VK_BLEND_OP_SUBTRACT
  };

 PolygonMode polygonMode;
 CullMode cullMode;
 FrontFace frontFace;
 bool depthClampEnable;
 bool rasterDiscardEnable;
 bool primitiveRestartEnable;
 bool depthBiasEnable;
 float lineWidth;

 SampleCount sampleCount;
 bool sampleShadingEnable;
 float minSampleShading;

 bool blendEnable;
 BlendFactor srcColorBlendFactor;
 BlendFactor dstColorBlendFactor;
 BlendOp colorBlendOp;
 BlendFactor srcAlphaBlendFactor;
 BlendFactor dstAlphaBlendFactor;
 BlendOp alphaBlendOp;

 std::vector<DynamicState> dynamicStates;

 InputTopology inputTopology;

 std::vector<const char*> validationLayers = {};

 const char* appName;
 uint32_t appVersion;

 const char* engineName;
 uint32_t engineVersion;

 uint32_t apiVersion;
};

extern V8_RenderConfig defaultRenderConfig;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);
