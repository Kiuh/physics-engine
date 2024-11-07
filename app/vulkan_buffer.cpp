#include "vulkan_buffer.h"

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanBuffer::create()
{
	createPrimaryBuffer();
	createStagingBuffer();
}

void VulkanBuffer::cleanup() const
{
	vkDestroyBuffer(device->logicalDevice, buffer, nullptr);
	vkFreeMemory(device->logicalDevice, bufferMemory, nullptr);

	vkDestroyBuffer(device->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(device->logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanBuffer::copyNewData()
{
	data_mutex->lock();

	// Copy data to staging buffer
	void* data;
	VK_CHECK(vkMapMemory(device->logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data));
	memcpy(data, sourceData, (size_t)bufferSize);
	vkUnmapMemory(device->logicalDevice, stagingBufferMemory);

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

	createBuffer(
		*device,
		createInfo,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		buffer,
		bufferMemory
	);
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

	createBuffer(
		*device,
		createInfo,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);
}

void VulkanBuffer::createBuffer(
	VulkanDevice device,
	VkBufferCreateInfo createInfo,
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer,
	VkDeviceMemory& bufferMemory
) const
{
	VK_CHECK(vkCreateBuffer(device.logicalDevice, &createInfo, nullptr, &buffer));
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.logicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(device.physicalDevice, memRequirements.memoryTypeBits, properties);

	VK_CHECK(vkAllocateMemory(device.logicalDevice, &allocInfo, nullptr, &bufferMemory));

	VK_CHECK(vkBindBufferMemory(device.logicalDevice, buffer, bufferMemory, 0));
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
	buffer.device = device;
	buffer.usageFlags = usageFlags;
	buffer.bufferSize = bufferSize;
	buffer.data_mutex = data_mutex;
	buffer.sourceData = sourceData;
	buffer.create();
	return buffer;
}
