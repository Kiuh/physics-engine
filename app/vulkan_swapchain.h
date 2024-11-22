#pragma once

#include "vk_image_builder.h"
#include "vma_usage.h"
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

struct VulkanSwapchain
{
	public:
	// Required
	VmaAllocator* vmaAllocator;
	VulkanDevice* device;
	VkSurfaceKHR surface{};
	WindowManager* windowManager;

	// Exposed
	VkSwapchainKHR instance{};
	std::vector<VkImage> images{};
	VkFormat imageFormat{};
	VkExtent2D extent{};

	std::vector<VkImageView> imageViews{};
	std::vector<VkFramebuffer> framebuffers{};
	std::vector<VkFramebuffer> uiFramebuffers{};

	// For MSAA
	VkImage colorImage{};
	VmaAllocation colorImageAllocation{};
	VkImageView colorImageView{};

	private:
	VkRenderPass* renderPass;
	VkRenderPass* uiRenderPass;

	public:
	void create();
	void addFrameBuffers(VkRenderPass* renderPass);
	void addUIFrameBuffers(VkRenderPass* uiRenderPass);
	void recreate();
	void cleanup() const;

	private:
	void createSwapChain();
	void createColorResources();
	void createImageViews();
	void createFramebuffers();
	void createUIFramebuffers();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
};