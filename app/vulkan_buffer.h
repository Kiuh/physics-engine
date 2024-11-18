#pragma once

#include "vulkan_device.h"
#include <mutex>
#include <vector>
#include <vulkan/vulkan_core.h>

uint32_t findMemoryType(
	VkPhysicalDevice physicalDevice,
	uint32_t typeFilter,
	VkMemoryPropertyFlags properties
);

struct VulkanBuffer
{
	private:
	VkBuffer stagingBuffer{};
	VkDeviceMemory stagingBufferMemory{};

	public:
	VulkanDevice* device = nullptr;
	VkBufferUsageFlags usageFlags{};
	VkDeviceSize bufferSize{};
	std::mutex* data_mutex = nullptr;
	const void* sourceData = nullptr;

	// Exposed
	VkBuffer buffer{};
	VkDeviceMemory bufferMemory{};

	void create();
	void cleanup() const;
	void copyNewData();

	private:
	void createPrimaryBuffer();
	void createStagingBuffer();
	void createBuffer(
		VulkanDevice device,
		VkBufferCreateInfo createInfo,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory
	) const;
	void copyBuffer(
		VulkanDevice device,
		VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		VkDeviceSize size
	) const;
};

struct VulkanBufferBuilder
{
	VulkanDevice* device;
	VkBufferUsageFlags usageFlags;
	VkDeviceSize bufferSize;
	std::mutex* data_mutex;
	const void* sourceData;

	VulkanBuffer build() const;
};