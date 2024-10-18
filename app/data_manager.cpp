#pragma once

#include "vertex_source.hpp"
#include "vertex.hpp"
#include "window_manager.cpp"
#include "box.cpp"
#include "circle.cpp"
#include <cstdint>
#include <mutex>
#include <vector>
#include "vulkan_vec.cpp"
#include "shape.hpp"

constexpr uint32_t pixelsPerUnit = 50;

class DataManager
{
	private:
	WindowManager* window = nullptr;

	public:
	std::vector<VertexSource*> dataSources{};
	vulkan_vec<Vertex> vertices{ };
	vulkan_vec<uint16_t> indexes{ };
	std::mutex data_mutex{};
	boost::signals2::signal<void()> dataStructureChanged{};

	DataManager(WindowManager* window)
	{
		this->window = window;
		vertices = vulkan_vec<Vertex>{ std::vector<Vertex>{{}} };
		indexes = vulkan_vec<uint16_t>{ std::vector<uint16_t>{0,1,2} };
	}

	void notifyStructureChanging()
	{
		createDataSpace();
		dataStructureChanged();
	}

	void createDataSpace()
	{
		data_mutex.lock();
		size_t vertices_count = 0;
		size_t indexes_count = 0;

		for (size_t i = 0; i < dataSources.size(); i++)
		{
			vertices_count += dataSources[i]->getVertexesCount();
			indexes_count += dataSources[i]->getIndexesCount();
		}

		vertices.vector.resize(vertices_count);
		indexes.vector.resize(indexes_count, 0);

		size_t start_space = 0;
		size_t ind_iter = 0;
		for (auto& source : dataSources)
		{
			for (auto& index : source->getIndexes())
			{
				indexes.vector[ind_iter] = static_cast<uint16_t>(start_space + index);
				ind_iter++;
			}
			start_space += source->getVertexesCount();
		}
		data_mutex.unlock();
	}

	void recalculateVertexes()
	{
		data_mutex.lock();
		uint32_t vert_iter = 0;
		for (auto& source : dataSources)
		{
			for (auto& vert : source->getVertexes())
			{
				vertices.vector[vert_iter] = vert;
				worldToScreen(vertices.vector[vert_iter], window, pixelsPerUnit);
				vert_iter++;
			}
		}
		data_mutex.unlock();
	}

	private:
	void worldToScreen(Vertex& vert, WindowManager* wp, uint32_t pixelsPerUnit)
	{
		auto screen = wp->getSize();
		screen /= 2.0f;
		vert.pos *= pixelsPerUnit;
		vert.pos /= screen;

		vert.pos.y *= -1;
	}
};