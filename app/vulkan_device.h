#pragma once

#include "validation_manager.h"
#include "vulkan_tools.h"
#include "log_tools.h"
#include <cstdint>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily{};
	std::optional<uint32_t> presentFamily{};
	std::optional<uint32_t> transferFamily{};

	bool isComplete() const;
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats{};
	std::vector<VkPresentModeKHR> presentModes{};
};

struct VulkanDevice
{
	public:
	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Required
	VkInstance instance{};
	VkSurfaceKHR surface{};
	ValidationManager* validationManager = nullptr;
	GraphicsEngineConfig config{};

	// Exposed
	VkPhysicalDevice physicalDevice{};
	VkDevice logicalDevice{};
	VkQueue graphicsQueue{};
	VkQueue presentQueue{};
	VkQueue transferQueue{};
	QueueFamilyIndices queueFamilyIndices{};
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

	VkCommandPool graphicsCommandPool{};
	VkCommandPool transferCommandPool{};
	std::vector<VkCommandBuffer> commandBuffers{};

	void init();
	void waitIdle() const;
	SwapChainSupportDetails getSwapChainSupportDetails() const;
	void cleanup() const;

	private:
	void createCommandBuffers();
	void createCommandPools();
	void createLogicalDevice();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;
	VkSampleCountFlagBits getMaxUsableSampleCount() const;
};