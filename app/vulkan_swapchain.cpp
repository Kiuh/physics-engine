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
#include "vk_image_builder.cpp"

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
	void create()
	{
		createSwapChain();
		createColorResources();
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
		createSwapChain();
		createColorResources();
		createImageViews();
		createFramebuffers();
	}

	void cleanup() const
	{
		vkDestroyImageView(device->logicalDevice, colorImageView, nullptr);
		vkDestroyImage(device->logicalDevice, colorImage, nullptr);
		vkFreeMemory(device->logicalDevice, colorImageMemory, nullptr);

		for (size_t i = 0; i < framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(device->logicalDevice, framebuffers[i], nullptr);
		}

		for (size_t i = 0; i < imageViews.size(); i++)
		{
			vkDestroyImageView(device->logicalDevice, imageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(device->logicalDevice, instance, nullptr);
	}

	private:
	void createSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = device->getSwapChainSupportDetails();

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0
			&& imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = device->queueFamilyIndices;
		uint32_t queueFamilyIndices[] = {
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};

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

		VK_CHECK(vkCreateSwapchainKHR(device->logicalDevice, &createInfo, nullptr, &instance));

		VK_CHECK(vkGetSwapchainImagesKHR(device->logicalDevice, instance, &imageCount, nullptr));
		images.resize(imageCount);
		VK_CHECK(vkGetSwapchainImagesKHR(device->logicalDevice, instance, &imageCount, images.data()));

		imageFormat = surfaceFormat.format;
		this->extent = extent;
	}

	void createColorResources()
	{
		createImage(
			device,
			extent.width,
			extent.height,
			1,
			imageFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			colorImage,
			colorImageMemory
		);
		colorImageView = createImageView(device, colorImage, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	void createImageViews()
	{
		imageViews.resize(images.size());

		for (size_t i = 0; i < images.size(); i++)
		{
			imageViews[i] = createImageView(device, images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}

	void createFramebuffers()
	{
		framebuffers.resize(imageViews.size());

		for (size_t i = 0; i < imageViews.size(); i++)
		{
			std::array<VkImageView, 2> attachments = {
				colorImageView,
				imageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = *renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = extent.width;
			framebufferInfo.height = extent.height;
			framebufferInfo.layers = 1;

			VK_CHECK(vkCreateFramebuffer(device->logicalDevice, &framebufferInfo, nullptr, &framebuffers[i]));
		}
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
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
		auto extendSize = windowManager->getSize();

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(extendSize.x),
			static_cast<uint32_t>(extendSize.y)
		};

		actualExtent.width = std::clamp(
			actualExtent.width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width
		);
		actualExtent.height = std::clamp(
			actualExtent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height
		);

		return actualExtent;
	}
};