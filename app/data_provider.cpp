#pragma once

#include "box.cpp"
#include "vertex.cpp"
#include "vertex_transformer.cpp"
#include <cstdint>
#include <mutex>
#include <vector>
#include <vulkan/vulkan_core.h>

class DataProvider
{
	private:
	VertexTransformer* vertexTransformer;
	std::mutex vertices_mutex;

	std::vector<Vertex> vertices = {};

	public:
	std::vector<Box> boxes = {};

	DataProvider(VertexTransformer* t)
	{
		this->vertexTransformer = t;
		prepareDataToDraw();
	}

	bool prepareDataToDraw()
	{
		vertices_mutex.lock();

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

		vertices_mutex.unlock();

		if (prev_size != vertices.size())
		{
			return true;
		}

		return false;
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