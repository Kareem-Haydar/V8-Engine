#include <Scene/Types.h>

void V8_StaticMesh::Init(V8_Context& context, const std::vector<V8_Vertex>& vertices, const std::vector<uint32_t>& indices) {
  this->vertices = vertices;
  this->indices = indices;

  VkBufferCreateInfo vertexBufferInfo {};
  vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vertexBufferInfo.size = vertices.size() * sizeof(V8_Vertex);
  vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo vertexAllocInfo {};
  vertexAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

  VK_CHECK(vmaCreateBuffer(context.allocator_, &vertexBufferInfo, &vertexAllocInfo, &vertexBuffer, &vertexBufferAllocation, nullptr));

  VkBufferCreateInfo indexBufferInfo {};
  indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  indexBufferInfo.size = indices.size() * sizeof(uint32_t);
  indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo indexAllocInfo {};
  indexAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

  VK_CHECK(vmaCreateBuffer(context.allocator_, &indexBufferInfo, &indexAllocInfo, &indexBuffer, &indexBufferAllocation, nullptr));

  allocator_ = &context.allocator_;

  UploadVertexData(context);
  UploadIndexData(context);
}

void V8_StaticMesh::UploadVertexData(V8_Context& context) {
  VkBuffer stagingBuffer;
  VmaAllocation stagingBufferAllocation;

  VkBufferCreateInfo bufferInfo {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = vertices.size() * sizeof(V8_Vertex);
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo {};
  allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

  VK_CHECK(vmaCreateBuffer(context.allocator_, &bufferInfo, &allocInfo, &stagingBuffer, &stagingBufferAllocation, nullptr));

  void* data;
  vmaMapMemory(context.allocator_, stagingBufferAllocation, &data);
  memcpy(data, vertices.data(), vertices.size() * sizeof(V8_Vertex));
  vmaUnmapMemory(context.allocator_, stagingBufferAllocation);

  VkCommandBuffer cmdBuffer;

  VkCommandBufferAllocateInfo cmdBufferAllocInfo {};
  cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdBufferAllocInfo.commandPool = context.commandPools_[context.graphicsQueueFamilyIndex_];
  cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdBufferAllocInfo.commandBufferCount = 1;

  VK_CHECK(vkAllocateCommandBuffers(context.device_, &cmdBufferAllocInfo, &cmdBuffer));

  VkCommandBufferBeginInfo cmdBufferBeginInfo {};
  cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo));

  VkBufferCopy copyRegion {};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = vertices.size() * sizeof(V8_Vertex);

  vkCmdCopyBuffer(cmdBuffer, stagingBuffer, vertexBuffer, 1, &copyRegion);

  VK_CHECK(vkEndCommandBuffer(cmdBuffer));

  VkSubmitInfo submitInfo {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmdBuffer;

  VK_CHECK(vkQueueSubmit(context.graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE));
  VK_CHECK(vkQueueWaitIdle(context.graphicsQueue_));

  vmaDestroyBuffer(context.allocator_, stagingBuffer, stagingBufferAllocation);
  vkFreeCommandBuffers(context.device_, context.commandPools_[context.graphicsQueueFamilyIndex_], 1, &cmdBuffer);
}

void V8_StaticMesh::UploadIndexData(V8_Context& context) {
  VkBuffer stagingBuffer;
  VmaAllocation stagingBufferAllocation;

  VkBufferCreateInfo bufferInfo {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = indices.size() * sizeof(uint32_t);
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo {};
  allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

  VK_CHECK(vmaCreateBuffer(context.allocator_, &bufferInfo, &allocInfo, &stagingBuffer, &stagingBufferAllocation, nullptr));

  void* data;
  vmaMapMemory(context.allocator_, stagingBufferAllocation, &data);
  memcpy(data, indices.data(), indices.size() * sizeof(uint32_t));
  vmaUnmapMemory(context.allocator_, stagingBufferAllocation);

  VkCommandBuffer cmdBuffer;

  VkCommandBufferAllocateInfo cmdBufferAllocInfo {};
  cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdBufferAllocInfo.commandPool = context.commandPools_[context.graphicsQueueFamilyIndex_];
  cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdBufferAllocInfo.commandBufferCount = 1;

  VK_CHECK(vkAllocateCommandBuffers(context.device_, &cmdBufferAllocInfo, &cmdBuffer));

  VkCommandBufferBeginInfo cmdBufferBeginInfo {};
  cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo));

  VkBufferCopy copyRegion {};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = indices.size() * sizeof(uint32_t);

  vkCmdCopyBuffer(cmdBuffer, stagingBuffer, indexBuffer, 1, &copyRegion);

  VK_CHECK(vkEndCommandBuffer(cmdBuffer));

  VkSubmitInfo submitInfo {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmdBuffer;

  VK_CHECK(vkQueueSubmit(context.graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE));
  VK_CHECK(vkQueueWaitIdle(context.graphicsQueue_));

  vmaDestroyBuffer(context.allocator_, stagingBuffer, stagingBufferAllocation);
  vkFreeCommandBuffers(context.device_, context.commandPools_[context.graphicsQueueFamilyIndex_], 1, &cmdBuffer);
}

V8_StaticMesh::~V8_StaticMesh() {
  if (vertexBuffer != VK_NULL_HANDLE && vertexBufferAllocation != VK_NULL_HANDLE) {
    vmaDestroyBuffer(*allocator_, vertexBuffer, vertexBufferAllocation);

    vertexBuffer = VK_NULL_HANDLE;
    vertexBufferAllocation = VK_NULL_HANDLE;
  }

  if (indexBuffer != VK_NULL_HANDLE && indexBufferAllocation != VK_NULL_HANDLE) {
    vmaDestroyBuffer(*allocator_, indexBuffer, indexBufferAllocation);

    indexBuffer = VK_NULL_HANDLE;
    indexBufferAllocation = VK_NULL_HANDLE;
  }
}
