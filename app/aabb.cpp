#pragma once

#include "color.cpp"
#include "vertex.cpp"
#include <glm.hpp>
#include <mutex>
#include <stdexcept>
#include <vector>

static constexpr const size_t BOX_VERTEX_COUNT = 6;

struct AABB
{
	// View properties
	glm::vec2 min{ 0,0 };
	glm::vec2 max{ 1,1 };
	Color color{ glm::vec3{ 1.0f, 1.0f, 1.0f } };

	glm::vec2 center() const
	{
		return min + ((max - min) / 2.0f);
	}

	glm::vec2 size() const
	{
		return max - min;
	}

	// Physics properties
	bool isStatic = true;
	glm::vec2 velocity{ 0,0 };

	static AABB make(glm::vec2 min, glm::vec2 max, Color color)
	{
		auto box = AABB{};
		box.setMinMax(min, max);
		box.setColor(color);
		return box;
	}

	void setMinMax(glm::vec2 min, glm::vec2 max)
	{
		if (min.x >= max.x || min.y >= max.y)
		{
			throw new std::runtime_error("invalid min and max values");
		}

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

	static bool isOverlaps(AABB& box1, AABB& box2)
	{
		if (
			box1.center().x < box2.center().x + box2.size().x &&
			box1.center().x + box1.size().x > box2.center().x &&
			box1.center().y < box2.center().y + box2.size().y &&
			box1.center().y + box1.size().y > box2.center().y
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};