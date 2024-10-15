#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "color.cpp"
#include "vertex.cpp"
#include "glm/glm.hpp"
#include <mutex>
#include <vector>
#include "shape.cpp"
#include <glm/gtx/vector_angle.hpp>

enum Corner
{
	LeftUp,
	RightUp,
	LeftDown,
	RightDown,
};

struct Box : public Shape
{
	private:
	glm::vec2 halfSize;
	Color color;

	public:
	Box(Transform& tr) : Shape(tr)
	{
		halfSize = glm::vec2{ 1.0f,1.0f };
		color = Color::randomColor();
	}

	void setHalfSize(glm::vec2 halfSize)
	{
		this->halfSize = halfSize;
	}

	glm::vec2 max() const
	{
		return transform.pos() + halfSize;
	}

	glm::vec2 min() const
	{
		return transform.pos() - halfSize;
	}

	glm::vec2 getPos(Corner corner) const
	{
		switch (corner)
		{
			case LeftUp: return { min().x, max().y };
			case RightUp: return max();
			case LeftDown: return min();
			case RightDown: return { max().x, min().y };
			default: throw std::runtime_error("Invalid input corner value");
		}
	}

	float getDegrees(Corner corner) const
	{
		auto vec = getPos(corner) - transform.pos();
		auto angle = glm::orientedAngle(glm::vec2(1.0f, 0), glm::normalize(vec));
		return glm::degrees(angle);
	}

	glm::vec2 size() const
	{
		return halfSize * 2.0f;
	}

	std::vector<Vertex> getVertexes() const
	{
		std::vector<Vertex> vertices(4);
		for (size_t i = 0; i < 4; i++)
		{
			vertices[i].color = color.getValue();
		}
		vertices[0].pos = min();
		vertices[1].pos = glm::vec2(min().x, max().y);
		vertices[2].pos = max();
		vertices[3].pos = glm::vec2(max().x, min().y);
		return vertices;
	}

	size_t getVertexesCount() const
	{
		return 4;
	}

	std::vector<size_t> getIndexes() const
	{
		return { 0, 1, 2, 2, 3, 0 };
	}

	size_t getIndexesCount() const
	{
		return 6;
	}
};