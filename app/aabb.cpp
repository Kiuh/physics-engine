#pragma once

#include "color.cpp"
#include "vertex.cpp"
#include <glm.hpp>
#include <mutex>
#include <vector>

static constexpr const size_t BOX_VERTEX_COUNT = 6;

struct AABB
{
	glm::vec2 min{ 0,0 };
	glm::vec2 max{ 1,1 };
	Color color{};

	void setMinMax(glm::vec2 min, glm::vec2 max)
	{
		this->min = min;
		this->max = max;
	}

	void setColor(Color color)
	{
		this->color = color;
	}

	std::vector<Vertex> calculateVertices() const
	{
		std::vector<Vertex> vertices(BOX_VERTEX_COUNT);

		for (size_t i = 0; i < BOX_VERTEX_COUNT; i++)
		{
			vertices[i].color = color.getValues();
		}
		vertices[0].pos = min;
		vertices[1].pos = glm::vec2(min.x, max.y);
		vertices[2].pos = max;
		vertices[3].pos = min;
		vertices[4].pos = max;
		vertices[5].pos = glm::vec2(max.x, min.y);

		return vertices;
	}

	void move(glm::vec2 delta)
	{
		min += delta;
		max += delta;
	}

	static bool isCollide(AABB a, AABB b)
	{
		if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
		if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
		return true;
	}
};