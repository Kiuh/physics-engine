#include "vulkan_swapchain.h"

void VulkanSwapchain::create()
{
	createSwapChain();
	createColorResources();
	createImageViews();
}

void VulkanSwapchain::addFrameBuffers(VkRenderPass* renderPass)
{
	this->renderPass = renderPass;
	createFramebuffers();
}

void VulkanSwapchain::recreate()
{
	cleanup();
	createSwapChain();
	createColorResources();
	createImageViews();
	createFramebuffers();
}

void VulkanSwapchain::cleanup() const
{
	vkDestroyImageView(device->logicalDevice, colorImageView, nullptr);
	vmaDestroyImage(*vmaAllocator, colorImage, colorImageAllocation);

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

void VulkanSwapchain::createSwapChain()
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

void VulkanSwapchain::createColorResources()
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = extent.width;
	imageInfo.extent.height = extent.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = imageFormat;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageInfo.samples = device->msaaSamples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo vmaAllocInfo{};
	vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	vmaCreateImage(*vmaAllocator, &imageInfo, &vmaAllocInfo, &colorImage, &colorImageAllocation, nullptr);

	colorImageView = createImageView(device, colorImage, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void VulkanSwapchain::createImageViews()
{
	imageViews.resize(images.size());

	for (size_t i = 0; i < images.size(); i++)
	{
		imageViews[i] = createImageView(device, images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void VulkanSwapchain::createFramebuffers()
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

VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
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

VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
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

VkExtent2D VulkanSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
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