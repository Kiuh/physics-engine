#pragma once

#include "vulkan_device.h"
#include "vulkan_tools.h"
#include "window_manager.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iosfwd>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "vk_image_builder.h"

struct VulkanSwapchain
{
	public:
	// Required
	VulkanDevice* device;
	VkSurfaceKHR surface;
	WindowManager* windowManager;

	// Exposed
	VkSwapchainKHR instance;
	std::vector<VkImage> images;
	VkFormat imageFormat;
	VkExtent2D extent;

	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;

	// For MSAA
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	private:
	VkRenderPass* renderPass;

	public:
	void create();
	void addFrameBuffers(VkRenderPass* renderPass);
	void recreate();
	void cleanup() const;

	private:
	void createSwapChain();
	void createColorResources();
	void createImageViews();
	void createFramebuffers();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
};