#pragma once

#include "aabb.cpp"
#include "vertex.cpp"
#include "window_manager.cpp"
#include <cstdint>
#include <mutex>
#include <vector>

class DataManager
{
	private:
	WindowManager* window;
	std::vector<Vertex> vertices = {};
	std::vector<uint16_t> indexes = {};
	uint32_t pixelsPerUnit = 50;

	public:
	std::vector<AABB> boxes = { AABB{} };
	std::mutex data_mutex{};

	DataManager(WindowManager* window)
	{
		this->window = window;
	}

	void addBox(AABB& box)
	{
		boxes.push_back(box);
	}

	void prepareDataToDraw()
	{
		data_mutex.lock();
		vertices.resize(boxes.size() * AABB_VERTEX_COUNT);
		indexes.resize(boxes.size() * AABB_INDEXES_COUNT);
		for (uint32_t i = 0; i < boxes.size(); i++)
		{
			auto box_vertices = boxes[i].getVertices();
			// Vertexes
			for (uint32_t j = 0; j < AABB_VERTEX_COUNT; j++)
			{
				auto ind = i * AABB_VERTEX_COUNT + j;
				vertices[ind] = box_vertices[j];
				worldToScreen(vertices[ind], window, pixelsPerUnit);
			}
			// Indexes
			for (uint32_t j = 0; j < AABB_INDEXES_COUNT; j++)
			{
				auto ind = i * AABB_INDEXES_COUNT + j;
				indexes[ind] = (uint16_t)(AABB_INDEXES[j] + i * AABB_VERTEX_COUNT);
			}
		}
		data_mutex.unlock();
	}

	uint32_t indexesCount() const
	{
		return static_cast<uint32_t>(indexes.size());
	}

	uint32_t triangleCount() const
	{
		return static_cast<uint32_t>(indexes.size() / 3);
	}

	VkDeviceSize getVkVertexesSize() const
	{
		return sizeof(Vertex) * vertices.size();
	}

	VkDeviceSize getVkIndexesSize() const
	{
		return sizeof(uint16_t) * indexes.size();
	}

	void* getVertexesPointer()
	{
		return vertices.data();
	}

	void* getIndexesPointer()
	{
		return indexes.data();
	}

	void worldToScreen(Vertex& vert, WindowManager* wp, uint32_t pixelsPerUnit)
	{
		auto screen = wp->getSize();
		screen /= 2.0f;
		vert.pos *= pixelsPerUnit;
		vert.pos /= screen;

		vert.pos.y *= -1;
	}
};