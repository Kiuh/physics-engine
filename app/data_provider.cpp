#pragma once

#include "box.cpp"
#include "vertex.cpp"
#include "vertex_transformer.cpp"
#include <cstdint>
#include <mutex>
#include <vector>
#include <vulkan/vulkan_core.h>

struct PrepareDrawDataResult
{
	bool IsContentChanged = false;
	bool IsCountChanged = false;
};

class DataProvider
{
	private:
	VertexTransformer* vertexTransformer;
	std::mutex vertices_mutex;

	vector<Vertex> vertices = {};

	public:
	vector<Box> boxes = {};

	DataProvider(VertexTransformer* t)
	{
		this->vertexTransformer = t;
		prepareDataToDraw();
	}

	PrepareDrawDataResult prepareDataToDraw()
	{
		vertices_mutex.lock();
		auto result = PrepareDrawDataResult{};

		auto prev_size = vertices.size();
		vertices.resize(boxes.size() * BOX_VERTEX_COUNT);
		for (uint32_t i = 0; i < boxes.size(); i++)
		{
			boxes[i].calculateVertices();
			for (uint32_t j = 0; j < BOX_VERTEX_COUNT; j++)
			{
				auto ind = i * BOX_VERTEX_COUNT + j;
				vertices[ind] = boxes[i].vertices[j];
				vertexTransformer->worldToScreen(&vertices[ind]);
			}
		}

		if (prev_size != vertices.size())
		{
			result.IsCountChanged = true;
		}

		result.IsContentChanged = true;
		vertices_mutex.unlock();
		return result;
	}

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