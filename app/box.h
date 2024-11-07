#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <stdexcept>
#include "color.h"
#include "vertex.h"
#include "glm/glm.hpp"
#include <vector>
#include "shape.h"
#include <glm/gtx/vector_angle.hpp>

enum Corner
{
	LeftUp,
	RightUp,
	LeftDown,
	RightDown,
};

class Box : public Shape
{
	public:
	glm::vec2 halfSize;
	Color color;

	Box(Transform& tr);
	void setHalfSize(glm::vec2 halfSize);
	glm::vec2 max() const;
	glm::vec2 min() const;
	glm::vec2 getPos(Corner corner) const;
	float getDegrees(Corner corner) const;
	glm::vec2 size() const;
	std::vector<glm::vec2> getPoints() const;

	std::vector<Vertex> getVertexes() const;
	size_t getVertexesCount() const;
	std::vector<size_t> getIndexes() const;
	size_t getIndexesCount() const;
};