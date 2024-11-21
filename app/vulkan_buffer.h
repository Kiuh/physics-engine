#pragma once

#include "vma_usage.h"
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
	VmaAllocation stagingBufferAllocation;

	public:
	VmaAllocator* vmaAllocator;
	VulkanDevice* device = nullptr;
	VkBufferUsageFlags usageFlags{};
	VkDeviceSize bufferSize{};
	std::mutex* data_mutex = nullptr;
	const void* sourceData = nullptr;

	// Exposed
	VkBuffer buffer{};
	VmaAllocation bufferAllocation{};

	void create();
	void cleanup() const;
	void copyNewData();

	private:
	void createPrimaryBuffer();
	void createStagingBuffer();
	void copyBuffer(
		VulkanDevice device,
		VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		VkDeviceSize size
	) const;
};

struct VulkanBufferBuilder
{
	VmaAllocator* vmaAllocator;
	VulkanDevice* device;
	VkBufferUsageFlags usageFlags;
	VkDeviceSize bufferSize;
	std::mutex* data_mutex;
	const void* sourceData;

	VulkanBuffer build() const;
};