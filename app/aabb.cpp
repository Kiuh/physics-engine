#pragma once

#include "color.cpp"
#include "vertex.cpp"
#include <glm.hpp>
#include <mutex>
#include <stdexcept>
#include <vector>
#include "vertex_source.cpp"

struct AABB : public VertexSource
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

	glm::vec2 extends() const
	{
		return size() / 2.0f;
	}

	static AABB* make(glm::vec2 min, glm::vec2 max, Color color)
	{
		auto* box = new AABB{};
		box->setMinMax(min, max);
		box->setColor(color);
		return box;
	}

	void setMinMax(glm::vec2 min, glm::vec2 max)
	{
		if (min.x >= max.x || min.y >= max.y)
		{
			throw new std::runtime_error("try set invalid min and max values in AABB");
		}

		this->min = min;
		this->max = max;
	}

	void setColor(Color color)
	{
		this->color = color;
	}

	std::vector<Vertex> getVertexes() const
	{
		std::vector<Vertex> vertices(4);
		for (size_t i = 0; i < 4; i++)
		{
			vertices[i].color = color.getValues();
		}
		vertices[0].pos = min;
		vertices[1].pos = glm::vec2(min.x, max.y);
		vertices[2].pos = max;
		vertices[3].pos = glm::vec2(max.x, min.y);
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

// Physics properties
	bool isStatic = true;
	glm::vec2 velocity{ 0,0 };
	float mass = 1.0f;

	void move(glm::vec2 delta)
	{
		min += delta;
		max += delta;
	}

	static bool isOverlaps(AABB& box1, AABB& box2)
	{
		if (box2.min.x >= box1.max.x || box2.max.x <= box1.min.x) return false;
		if (box2.min.y >= box1.max.y || box2.max.y <= box1.min.y) return false;
		return true;
	}

	// Resulting vector moves box1
	static glm::vec2 getShortestOverlap(AABB& box1, AABB& box2)
	{
		float x_overlap = getSegmentOverlap(box1.min.x, box1.max.x, box2.min.x, box2.max.x);
		float y_overlap = getSegmentOverlap(box1.min.y, box1.max.y, box2.min.y, box2.max.y);

		if (glm::abs(x_overlap) < glm::abs(y_overlap))
		{
			return glm::vec2(x_overlap, 0);
		}
		else
		{
			return glm::vec2(0, y_overlap);
		}
	}

	static float getSegmentOverlap(float a1, float b1, float a2, float b2)
	{
		float len1 = b1 - a1;
		float len2 = b2 - a2;

		float m1 = a1 + len1 / 2.0f; // middle of 1 seg
		float m2 = a2 + len2 / 2.0f; // middle of 2 seg

		if (a1 >= a2 && b1 <= b2) // fully inside
		{
			if (m1 > m2)
			{
				return len1;
			}
			else
			{
				return -len1;
			}
		}

		if (a1 <= a2 && b1 >= a1 && b1 <= b2) // overlaps only left point
		{
			return -(b1 - a2);
		}

		if (a1 >= a2 && a1 <= b2 && b1 >= b2) // overlaps only right point
		{
			return b2 - a1;
		}

		// fully overlaps
		if (m1 > m2)
		{
			return len2;
		}
		else
		{
			return -len2;
		}
	}
};