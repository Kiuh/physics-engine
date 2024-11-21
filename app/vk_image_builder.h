#pragma once

#include "vulkan_device.h"
#include <vulkan/vulkan_core.h>

VkImageView createImageView(
	VulkanDevice* device,
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspectFlags,
	uint32_t mipLevels
);