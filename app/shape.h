#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "color.h"
#include "transform.h"
#include "vertex_source.h"
#include "vec2_tools.h"

struct Shape : public VertexSource
{
	Transform& tr;
	Color color;
	std::vector<glm::vec2> localPoints{};

	Shape(Transform& tr);

	std::vector<glm::vec2> getWorldPoints() const;

	size_t getPolygonsCount() const;
	std::vector<Vertex> getVertexes() const;
	size_t getVertexesCount() const;
	std::vector<size_t> getIndexes() const;
	size_t getIndexesCount() const;
};