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
		data_mutex.lock();
		boxes.push_back(box);
		data_mutex.unlock();
	}

	void prepareDataToDraw()
	{
		data_mutex.lock();
		auto prev_size = vertices.size();
		vertices.resize(boxes.size() * BOX_VERTEX_COUNT);
		for (uint32_t i = 0; i < boxes.size(); i++)
		{
			auto box_vertices = boxes[i].calculateVertices();
			for (uint32_t j = 0; j < BOX_VERTEX_COUNT; j++)
			{
				auto ind = i * BOX_VERTEX_COUNT + j;
				vertices[ind] = box_vertices[j];
				worldToScreen(vertices[ind], window, pixelsPerUnit);
			}
		}
		data_mutex.unlock();
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

	void worldToScreen(Vertex& vert, WindowManager* wp, uint32_t pixelsPerUnit)
	{
		auto screen = wp->getSize();
		screen /= 2.0f;
		vert.pos *= pixelsPerUnit;
		vert.pos /= screen;

		vert.pos.y *= -1;
	}
};