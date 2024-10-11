#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

template<typename T>
struct vulkan_vec
{
	std::vector<T> vector{};

	vulkan_vec()
	{
		vector = {};
	}

	vulkan_vec(std::vector<T> vec)
	{
		vector = vec;
	}

	VkDeviceSize getVkSize() const
	{
		return sizeof(T) * vector.size();
	}

	const void* getDataPointer()
	{
		return vector.data();
	}

	uint32_t size()
	{
		return static_cast<uint32_t>(vector.size());
	}
};