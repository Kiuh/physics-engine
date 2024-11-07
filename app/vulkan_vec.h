#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

template<typename T>
struct vulkan_vec
{
	std::vector<T> vector{};

	vulkan_vec();
	vulkan_vec(std::vector<T> vec);
	VkDeviceSize getVkSize() const;
	const void* getDataPointer();
	uint32_t size();
};