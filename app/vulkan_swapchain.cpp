#pragma once

#include "vulkan_device.cpp"
#include "vulkan_tools.cpp"
#include "window_manager.cpp"
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
	VulkanDevice* vulkanDevice;
	VkSurfaceKHR surface;
	WindowManager* windowManager;

	// Exposed
	VkSwapchainKHR instance;
	std::vector<VkImage> images;
	VkFormat imageFormat;
	VkExtent2D extent;

	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;

	private:
	VkRenderPass* renderPass;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR presentMode;
	VkSwapchainCreateInfoKHR createInfo{};
	uint32_t imageCount;

	public:
	void create()
	{
		createSwapChain();
		createImageViews();
	}

	void addFrameBuffers(VkRenderPass* renderPass)
	{
		this->renderPass = renderPass;
		createFramebuffers();
	}

	void recreate()
	{
		cleanup();
		recreateSwapChain();
		createImageViews();
		createFramebuffers();
	}

	void cleanup() const
	{
		for (size_t i = 0; i < framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(vulkanDevice->logicalDevice, framebuffers[i], nullptr);
		}

		for (size_t i = 0; i < imageViews.size(); i++)
		{
			vkDestroyImageView(vulkanDevice->logicalDevice, imageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(vulkanDevice->logicalDevice, instance, nullptr);
	}

	private:
	void recreateSwapChain()
	{
		VkExtent2D extent = chooseSwapExtent(vulkanDevice->swapChainSupport.capabilities);
		createInfo.imageExtent = extent;

		VK_CHECK(vkCreateSwapchainKHR(vulkanDevice->logicalDevice, &createInfo, nullptr, &instance));

		vkGetSwapchainImagesKHR(vulkanDevice->logicalDevice, instance, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(vulkanDevice->logicalDevice, instance, &imageCount, images.data());

		imageFormat = surfaceFormat.format;
		extent = extent;
	}

	void createSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = vulkanDevice->swapChainSupport;

		surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = vulkanDevice->queueFamilyIndices;
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VK_CHECK(vkCreateSwapchainKHR(vulkanDevice->logicalDevice, &createInfo, nullptr, &instance));

		vkGetSwapchainImagesKHR(vulkanDevice->logicalDevice, instance, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(vulkanDevice->logicalDevice, instance, &imageCount, images.data());

		imageFormat = surfaceFormat.format;
		this->extent = extent;
	}

	void createImageViews()
	{
		imageViews.resize(images.size());

		for (size_t i = 0; i < images.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = imageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(vulkanDevice->logicalDevice, &createInfo, nullptr, &imageViews[i]));
		}
	}

	void createFramebuffers()
	{
		framebuffers.resize(imageViews.size());

		for (size_t i = 0; i < imageViews.size(); i++)
		{
			VkImageView attachments[] = {
				imageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = *renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = extent.width;
			framebufferInfo.height = extent.height;
			framebufferInfo.layers = 1;

			VK_CHECK(vkCreateFramebuffer(vulkanDevice->logicalDevice, &framebufferInfo, nullptr, &framebuffers[i]))
		}
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
	{
		if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
		{
			return capabilities.currentExtent;
		}
		else
		{
			auto extendSize = windowManager->getSize();

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(extendSize.x),
				static_cast<uint32_t>(extendSize.y)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
};