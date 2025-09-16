#include <Renderer/VulkanTypes.h>
#include <Core/WindowManager.h>

Renderer::SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  Renderer::SwapchainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities_);

  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats_.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats_.data());
  }

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes_.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes_.data());
  }

  return details;
}


VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t windowID) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    SDL_GetWindowSize(Core::windowManager.GetWindow(windowID).Get(), &width, &height);

    VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
  }
}

void Renderer::Swapchain::Init(const Device& device, const Surface& surface, uint32_t windowID) {
  SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device.physicalDevice_, surface.surface_);

  VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapchainSupport.formats_);
  VkPresentModeKHR presentMode = ChoosePresentMode(swapchainSupport.presentModes_);
  VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities_, windowID);

  uint32_t imageCount = swapchainSupport.capabilities_.minImageCount + 1;
  if (swapchainSupport.capabilities_.maxImageCount > 0 && imageCount > swapchainSupport.capabilities_.maxImageCount) 
    imageCount = swapchainSupport.capabilities_.maxImageCount;

  VkSwapchainCreateInfoKHR createInfo {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface.surface_;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamilyIndices[] = { device.graphicsQueueFamilyIndex_, device.presentQueueFamilyIndex_ };

  if (device.graphicsQueueFamilyIndex_ != device.presentQueueFamilyIndex_) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  createInfo.preTransform = swapchainSupport.capabilities_.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device.device_, &createInfo, nullptr, &swapchain_) != VK_SUCCESS)
    V_FATAL("Failed to create swapchain");

  vkGetSwapchainImagesKHR(device.device_, swapchain_, &imageCount, nullptr);
  images_.resize(imageCount);
  vkGetSwapchainImagesKHR(device.device_, swapchain_, &imageCount, images_.data());

  imageViews_.resize(imageCount);
  for (size_t i = 0; i < imageCount; i++) {
    VkImageViewCreateInfo viewInfo {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = images_[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = surfaceFormat.format;
    viewInfo.subresourceRange = {};
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    if (vkCreateImageView(device.device_, &viewInfo, nullptr, &imageViews_[i]) != VK_SUCCESS)
      V_FATAL("Failed to create image views");
  }

  imageFormat_ = surfaceFormat.format;
  extent_ = extent;
  device_ = device.device_;
}

void Renderer::Swapchain::GetFramebuffers(const RenderPass& renderPass) {
  framebuffers_.resize(images_.size());

  for (size_t i = 0; i < images_.size(); i++) {
    VkImageView attachments[] = { imageViews_[i] };

    VkFramebufferCreateInfo framebufferInfo {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass.renderPass_;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent_.width;
    framebufferInfo.height = extent_.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device_, &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS)
      V_FATAL("Failed to create framebuffer");
  }
}

Renderer::Swapchain::~Swapchain() {
  for (auto imageView : imageViews_) {
    if (imageView != VK_NULL_HANDLE)
      vkDestroyImageView(device_, imageView, nullptr);
  }

  for (auto framebuffer : framebuffers_) {
    if (framebuffer != VK_NULL_HANDLE)
      vkDestroyFramebuffer(device_, framebuffer, nullptr);
  }

  if (swapchain_ != VK_NULL_HANDLE)
    vkDestroySwapchainKHR(device_, swapchain_, nullptr);
} 
