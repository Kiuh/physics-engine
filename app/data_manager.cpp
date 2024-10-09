#pragma once

#include "vertex_source.cpp"
#include "vertex.cpp"
#include "window_manager.cpp"
#include "aabb.cpp"
#include "circle.cpp"
#include <cstdint>
#include <mutex>
#include <vector>

class DataManager
{
	private:
	WindowManager* window = nullptr;
	std::vector<Vertex> vertices{};
	std::vector<uint16_t> indexes{};
	uint32_t pixelsPerUnit = 50;
	std::vector<VertexSource*> dataSources{};

	public:
	std::mutex data_mutex{};
	boost::signals2::signal<void()> dataStructureChanged{};
	std::vector<AABB*> boxes{};
	std::vector<Circle*> circles{};

	DataManager(WindowManager* window)
	{
		this->window = window;
	}

	void clearObjects()
	{
		dataSources.clear();
		for (size_t i = 0; i < boxes.size(); i++)
		{
			delete boxes[i];
		}
		for (size_t i = 0; i < circles.size(); i++)
		{
			delete circles[i];
		}
		boxes.clear();
		circles.clear();
	}

	void addAABB(AABB* box)
	{
		dataSources.push_back(box);
		boxes.push_back(box);
	}

	void addCircle(Circle* circle)
	{
		dataSources.push_back(circle);
		circles.push_back(circle);
	}

	void notifyStructureChanging()
	{
		createDataSpace();
		dataStructureChanged();
	}

	void createDataSpace() // resize and add correct indexes
	{
		data_mutex.lock();
		size_t vertices_count = 0;
		size_t indexes_count = 0;

		for (size_t i = 0; i < dataSources.size(); i++)
		{
			vertices_count += dataSources[i]->getVertexesCount();
			indexes_count += dataSources[i]->getIndexesCount();
		}

		vertices.resize(vertices_count, Vertex{});
		indexes.resize(indexes_count, 0);

		size_t start_space = 0;
		size_t ind_iter = 0;
		for (auto& source : dataSources)
		{
			for (auto& index : source->getIndexes())
			{
				indexes[ind_iter] = static_cast<uint16_t>(start_space + index);
				ind_iter++;
			}
			start_space += source->getVertexesCount();
		}
		data_mutex.unlock();
	}

	void recalculateVertexes() // copy vertexes from boxes
	{
		data_mutex.lock();
		uint32_t vert_iter = 0;
		for (auto& source : dataSources)
		{
			for (auto& vert : source->getVertexes())
			{
				vertices[vert_iter] = vert;
				worldToScreen(vertices[vert_iter], window, pixelsPerUnit);
				vert_iter++;
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

	const void* getVertexesPointer()
	{
		return vertices.data();
	}

	const void* getIndexesPointer()
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

	~DataManager()
	{
		clearObjects();
	}
};