#pragma once

#include "vulkan_buffer.h"
#include "vulkan_device.h"
#include <vulkan/vulkan_core.h>

VkImageView createImageView(
	VulkanDevice* device,
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspectFlags,
	uint32_t mipLevels
);

void createImage(
	VulkanDevice* device,
	uint32_t width,
	uint32_t height,
	uint32_t mipLevels,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage& image,
	VkDeviceMemory& imageMemory
);