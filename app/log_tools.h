#pragma once

#include <vector>
#include <iostream>
#include <magic_enum.hpp>
#include <vulkan/vulkan_core.h>

void logDeviceProperties(VkPhysicalDevice& device);
void logQueueFamilyProperties(VkQueueFamilyProperties queueProps);
void logDeviceQueueFamilies(VkPhysicalDevice device);
void logAllVkPhysicalDevices(std::vector<VkPhysicalDevice> devices);