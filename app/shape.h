#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "color.h"
#include "transform.h"
#include "vec2_tools.h"
#include "vertex_source.h"

struct Shape : public VertexSource
{
	private:
	std::vector<glm::vec2> localPoints{};

	public:
	Transform& tr;
	Color color;

	Shape(Transform& tr);

	std::vector<glm::vec2> getLocalPoints() const;
	void setLocalPoints(std::vector<glm::vec2> points);
	std::vector<glm::vec2> getWorldPoints() const;

	void correctCenter();

	size_t getPolygonsCount() const;
	std::vector<Vertex> getVertexes() const;
	size_t getVertexesCount() const;
	std::vector<size_t> getIndexes() const;
	size_t getIndexesCount() const;
};