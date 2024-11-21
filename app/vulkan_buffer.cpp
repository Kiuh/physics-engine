#include "vulkan_buffer.h"

void VulkanBuffer::create()
{
	createPrimaryBuffer();
	createStagingBuffer();
}

void VulkanBuffer::cleanup() const
{
	vmaDestroyBuffer(*vmaAllocator, buffer, bufferAllocation);
	vmaDestroyBuffer(*vmaAllocator, stagingBuffer, stagingBufferAllocation);
}

void VulkanBuffer::copyNewData()
{
	data_mutex->lock();

	// Copy data to staging buffer
	void* mappedData;
	vmaMapMemory(*vmaAllocator, stagingBufferAllocation, &mappedData);
	memcpy(mappedData, sourceData, bufferSize);
	vmaUnmapMemory(*vmaAllocator, stagingBufferAllocation);

	// Copy data from staging buffer to working buffer
	copyBuffer(*device, stagingBuffer, buffer, bufferSize);

	data_mutex->unlock();
}

void VulkanBuffer::createPrimaryBuffer()
{
	auto queueFamilyIndices = std::vector<uint32_t>
	{
		device->queueFamilyIndices.graphicsFamily.value(),
		device->queueFamilyIndices.transferFamily.value(),
	};

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = bufferSize;
	createInfo.usage = usageFlags;
	createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	createInfo.queueFamilyIndexCount = (uint32_t)queueFamilyIndices.size();
	createInfo.pQueueFamilyIndices = queueFamilyIndices.data();

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	VK_CHECK(vmaCreateBuffer(*vmaAllocator, &createInfo, &allocInfo, &buffer, &bufferAllocation, nullptr));
}

void VulkanBuffer::createStagingBuffer()
{
	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = bufferSize;
	createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 1;
	createInfo.pQueueFamilyIndices = &device->queueFamilyIndices.transferFamily.value();

	VmaAllocationCreateInfo vmaAllocInfo = {};
	vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VK_CHECK(vmaCreateBuffer(*vmaAllocator, &createInfo, &vmaAllocInfo, &stagingBuffer, &stagingBufferAllocation, nullptr));
}

void VulkanBuffer::copyBuffer(VulkanDevice device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = device.transferCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	VK_CHECK(vkAllocateCommandBuffers(device.logicalDevice, &allocInfo, &commandBuffer));

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	VK_CHECK(vkEndCommandBuffer(commandBuffer));

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VK_CHECK(vkQueueSubmit(device.transferQueue, 1, &submitInfo, VK_NULL_HANDLE));
	VK_CHECK(vkQueueWaitIdle(device.transferQueue));

	vkFreeCommandBuffers(device.logicalDevice, device.transferCommandPool, 1, &commandBuffer);
}


VulkanBuffer VulkanBufferBuilder::build() const
{
	VulkanBuffer buffer{};
	buffer.vmaAllocator = vmaAllocator;
	buffer.device = device;
	buffer.usageFlags = usageFlags;
	buffer.bufferSize = bufferSize;
	buffer.data_mutex = data_mutex;
	buffer.sourceData = sourceData;
	buffer.create();
	return buffer;
}
