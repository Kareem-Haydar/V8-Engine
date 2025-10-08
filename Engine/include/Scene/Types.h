#pragma once

#include <Core/Context.h>
#include <Core/Utils.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Matrix4 = glm::mat4;

struct V8_Vertex {
  Vector3 position;
  Vector3 normal;
  Vector3 color;
  Vector2 uv;

  static VkVertexInputBindingDescription GetBindingDescription() {
    VkVertexInputBindingDescription bindingDescription {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(V8_Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions {};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(V8_Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(V8_Vertex, normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(V8_Vertex, color);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(V8_Vertex, uv);

    return attributeDescriptions;
  }
};

struct V8_StaticMesh {
  private:
    VmaAllocator* allocator_;

    void UploadVertexData(V8_Context& context);
    void UploadIndexData(V8_Context& context);

  public:
    std::vector<V8_Vertex> vertices;
    std::vector<uint32_t> indices;

    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    VmaAllocation vertexBufferAllocation = VK_NULL_HANDLE;
    VkBuffer vertexBuffer = VK_NULL_HANDLE;

    VmaAllocation indexBufferAllocation = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;

    void Init(V8_Context& context, const std::vector<V8_Vertex>& vertices, const std::vector<uint32_t>& indices);
    void UploadData(V8_Context& context) {
      UploadVertexData(context);
      UploadIndexData(context);
    }

    ~V8_StaticMesh();
};
