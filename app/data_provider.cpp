#pragma once

#include "box.cpp"
#include "vertex.cpp"
#include "vertex_transformer.cpp"
#include <cstdint>
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

	vector<Box> boxes = {
		{
			{
				{ 0, 2 },
				{ 2, 2 },
				{ 2, 0 },
				{ 0, 0 },
			},
			{ 1.0f, 0.0f, 0.0f }
		},
	};

	vector<Vertex> vertices = {};

	public:
	DataProvider(VertexTransformer* t)
	{
		this->vertexTransformer = t;
		prepareDataToDraw();
	}

	PrepareDrawDataResult prepareDataToDraw()
	{
		auto result = PrepareDrawDataResult{};

		auto prev_size = vertices.size();
		vertices.resize(boxes.size() * BOX_VERTEX_COUNT);
		for (size_t i = 0; i < boxes.size(); i++)
		{
			boxes[i].calculateVertices();
			for (size_t j = 0; j < boxes[i].vertices.size(); j++)
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