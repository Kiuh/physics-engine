#include "data_manager.h"

DataManager::DataManager(WindowManager& window) : window(window)
{
	simplex_points = vulkan_vec<Vertex>{ std::vector<Vertex>{{}} };
	indexes = vulkan_vec<uint16_t>{ std::vector<uint16_t>{0,1,2} };
	window.mouseScroll.connect(boost::bind(&DataManager::handleMouseScroll, this, boost::placeholders::_1));
	window.pressedMouseMoved.connect(boost::bind(&DataManager::handlePressedMouseMove, this, boost::placeholders::_1));
}

void DataManager::setPixelPerUnit(float ppu)
{
	pixelsPerUnit = glm::clamp(ppu, MIN_PIXELS_PER_UNIT, MAX_PIXELS_PER_UNIT);
}

void DataManager::handleMouseScroll(float value)
{
	setPixelPerUnit(pixelsPerUnit + value);
}

void DataManager::handlePressedMouseMove(glm::vec2 delta)
{
	zeroShift -= delta * MOUSE_SENSETIVE;
}

void DataManager::notifyStructureChanging()
{
	createDataSpace();
	dataStructureChanged();
}

void DataManager::createDataSpace()
{
	data_mutex.lock();
	size_t vertices_count = 0;
	size_t indexes_count = 0;

	for (size_t i = 0; i < dataSources.size(); i++)
	{
		vertices_count += dataSources[i]->getVertexesCount();
		indexes_count += dataSources[i]->getIndexesCount();
	}

	simplex_points.vector.resize(vertices_count);
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

void DataManager::recalculateVertexes()
{
	data_mutex.lock();
	uint32_t vert_iter = 0;
	for (auto& source : dataSources)
	{
		for (auto& vert : source->getVertexes())
		{
			simplex_points.vector[vert_iter] = vert;
			worldToScreen(simplex_points.vector[vert_iter], window, pixelsPerUnit);
			vert_iter++;
		}
	}
	data_mutex.unlock();
}

void DataManager::worldToScreen(Vertex& vert, WindowManager& wp, float pixelsPerUnit) const
{
	auto screen = wp.getSize();
	screen /= 2.0f;
	vert.pos += zeroShift * glm::vec2{ 1,-1 };
	vert.pos *= pixelsPerUnit;
	vert.pos /= screen;

	vert.pos.y *= -1;
}
