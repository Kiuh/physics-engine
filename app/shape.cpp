#include "shape.h"

Shape::Shape(Transform& tr) : tr(tr) {}

std::vector<glm::vec2> Shape::getWorldPoints() const
{
	std::vector<glm::vec2> result{};
	for (auto& p : localPoints)
	{
		glm::vec2 point{ p };
		rotateVec2(point, glm::radians(tr.rot()));
		result.push_back(point + tr.pos());
	}
	return result;
}

size_t Shape::getPolygonsCount() const
{
	return localPoints.size() - 2;
}

std::vector<Vertex> Shape::getVertexes() const
{
	std::vector<Vertex> result{};
	auto worldPoints = getWorldPoints();
	for (size_t i = 0; i < worldPoints.size(); i++)
	{
		Vertex vert(worldPoints[i], color.getValue());
		result.push_back(vert);
	}
	return result;
}

std::vector<size_t> Shape::getIndexes() const
{
	std::vector<size_t> result{};
	size_t ind = 1;
	for (size_t i = 0; i < getPolygonsCount(); i++)
	{
		result.push_back(0);
		result.push_back(ind);
		result.push_back(ind + 1);
		ind += 1;
	}
	return result;
}

size_t Shape::getVertexesCount() const
{
	return localPoints.size();
}

size_t Shape::getIndexesCount() const
{
	return getPolygonsCount() * 3;
}