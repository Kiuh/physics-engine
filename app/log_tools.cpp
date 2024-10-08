#pragma once

#include <vector>
#include <iostream>
#include "magic_enum/magic_enum.hpp"
#include <vulkan/vulkan_core.h>

static void logDeviceProperties(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties prop{};
	vkGetPhysicalDeviceProperties(device, &prop);
	std::cerr << "Name: " << prop.deviceName << std::endl;
	std::cerr << "Type: " << prop.deviceType << std::endl;
	std::cerr << "Driver version: " << prop.driverVersion << std::endl;
}

static void logQueueFamilyProperties(VkQueueFamilyProperties queueProps)
{
	constexpr auto queueFlags = magic_enum::enum_values<VkQueueFlagBits>();

	std::cerr << "QueueCount: " << queueProps.queueCount << std::endl;
	std::cerr << "Flags:" << std::endl;
	for (auto& flag : queueFlags)
	{
		if (queueProps.queueFlags & flag)
		{
			std::cerr << std::string(magic_enum::enum_name(flag)) << std::endl;
		}
	}
	std::cerr << std::endl;
}

static void logDeviceQueueFamilies(VkPhysicalDevice device)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	std::cerr << "Device QueueFamily Count: " << queueFamilyCount << std::endl;
	int i = 0;
	for (auto& queueProps : queueFamilies)
	{
		std::cerr << ">>QueueFamilyIndex: " << i << std::endl;
		logQueueFamilyProperties(queueProps);
		i++;
	}
	std::cerr << std::endl;
}

static void logAllVkPhysicalDevices(std::vector<VkPhysicalDevice> devices)
{
	std::cerr << "Number of physical devices available: " << devices.size() << std::endl;
	for (auto& device : devices)
	{
		logDeviceProperties(device);
		logDeviceQueueFamilies(device);
	}
}