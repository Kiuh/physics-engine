#pragma once

#include "vertex.cpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

class DrawDataProvider
{
	private:
	std::vector<Vertex> vertices = {
		{
			{0.0f, -0.5f},
			{1.0f, 0.0f, 0.0f},
		},
		{
			{0.5f, 0.5f},
			{0.0f, 1.0f, 0.0f},
		},
		{
			{-0.5f, 0.5f},
			{0.0f, 0.0f, 1.0f},
		},
		{
			{-0.8f, 0.8f},
			{0.0f, 0.0f, 1.0f},
		},
		{
			{-0.1f, 0.1f},
			{0.0f, 1.0f, 1.0f},
		},
		{
			{0.1f, 0.1f},
			{1.0f, 1.0f, 1.0f},
		},
	};

	public:
	uint32_t verticesSize() const
	{
		return static_cast<uint32_t>(vertices.size());
	}

	uint32_t triangleCount() const
	{
		return verticesSize() / 3;
	}

	VkDeviceSize getVkSize() const
	{
		return sizeof(Vertex) * vertices.size();
	}

	void* getDataPointer()
	{
		return vertices.data();
	}
};