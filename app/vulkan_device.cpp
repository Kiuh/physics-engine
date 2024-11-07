#include "vulkan_device.h"

bool QueueFamilyIndices::isComplete() const
{
	return graphicsFamily.has_value()
		&& presentFamily.has_value()
		&& transferFamily.has_value();
}

void VulkanDevice::init()
{
	pickPhysicalDevice();
	createLogicalDevice();
	queueFamilyIndices = findQueueFamilies(physicalDevice);
	createCommandPools();
	createCommandBuffers();

	if (config.logging)
	{
		VkPhysicalDeviceProperties prop{};
		vkGetPhysicalDeviceProperties(physicalDevice, &prop);
		std::cerr << "+++Picked device: " << prop.deviceName << std::endl;
		std::cerr << "graphicsQueue: " << queueFamilyIndices.graphicsFamily.value() << std::endl;
		std::cerr << "presentQueue: " << queueFamilyIndices.presentFamily.value() << std::endl;
		std::cerr << "transferQueue: " << queueFamilyIndices.transferFamily.value() << std::endl;
		std::cerr << std::endl;
	}
}

void VulkanDevice::waitIdle() const
{
	vkDeviceWaitIdle(logicalDevice);
}

SwapChainSupportDetails VulkanDevice::getSwapChainSupportDetails() const
{
	return querySwapChainSupport(physicalDevice);
}

void VulkanDevice::cleanup() const
{
	vkDestroyCommandPool(logicalDevice, graphicsCommandPool, nullptr);
	vkDestroyCommandPool(logicalDevice, transferCommandPool, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
}


void VulkanDevice::createCommandBuffers()
{
	commandBuffers.resize(config.maxFramesInFlight);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = graphicsCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	VK_CHECK(vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()));
}

void VulkanDevice::createCommandPools()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VK_CHECK(vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &graphicsCommandPool));

	VkCommandPoolCreateInfo poolInfo1{};
	poolInfo1.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo1.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	poolInfo1.queueFamilyIndex = queueFamilyIndices.transferFamily.value();

	VK_CHECK(vkCreateCommandPool(logicalDevice, &poolInfo1, nullptr, &transferCommandPool));
}

void VulkanDevice::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.transferFamily.value(),
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.fillModeNonSolid = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	validationManager->addMessengerToDevice(createInfo);

	VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice));

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
	vkGetDeviceQueue(logicalDevice, indices.transferFamily.value(), 0, &transferQueue);
}

void VulkanDevice::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

	if (config.logging)
	{
		logAllVkPhysicalDevices(devices);
	}

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			physicalDevice = device;
			msaaSamples = getMaxUsableSampleCount();
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()));

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) const
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// Find Graphics family
	for (uint32_t i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
			break;
		}
	}

	// Find Present family
	for (uint32_t i = 0; i < queueFamilies.size(); i++)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
			break;
		}
	}

	// Find Graphics family
	for (uint32_t i = 0; i < queueFamilies.size(); i++)
	{
		if (i == indices.graphicsFamily.value())
		{
			continue;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			indices.transferFamily = i;
			break;
		}
	}

	return indices;
}

SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device) const
{
	SwapChainSupportDetails details;

	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities));

	uint32_t formatCount;
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data()));
	}

	uint32_t presentModeCount;
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data()));
	}

	return details;
}

VkSampleCountFlagBits VulkanDevice::getMaxUsableSampleCount() const
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

	if (counts & VK_SAMPLE_COUNT_64_BIT)
	{
		return VK_SAMPLE_COUNT_64_BIT;
	}
	if (counts & VK_SAMPLE_COUNT_32_BIT)
	{
		return VK_SAMPLE_COUNT_32_BIT;
	}
	if (counts & VK_SAMPLE_COUNT_16_BIT)
	{
		return VK_SAMPLE_COUNT_16_BIT;
	}
	if (counts & VK_SAMPLE_COUNT_8_BIT)
	{
		return VK_SAMPLE_COUNT_8_BIT;
	}
	if (counts & VK_SAMPLE_COUNT_4_BIT)
	{
		return VK_SAMPLE_COUNT_4_BIT;
	}
	if (counts & VK_SAMPLE_COUNT_2_BIT)
	{
		return VK_SAMPLE_COUNT_2_BIT;
	}

	return VK_SAMPLE_COUNT_1_BIT;
}