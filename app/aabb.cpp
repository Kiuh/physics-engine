#pragma once

#include "color.cpp"
#include "vertex.cpp"
#include "glm/glm.hpp"
#include <mutex>
#include <stdexcept>
#include <vector>
#include "shape.cpp"

struct AABB : public Shape
{
	private:
	glm::vec2 halfSize;
	Color color;

	public:
	AABB()
	{
		halfSize = glm::vec2{ 0.5f,0.5f };
		color = Color::randomColor();
	}

	void setHalfSize(glm::vec2 halfSize)
	{
		this->halfSize = halfSize;
	}

	glm::vec2 max() const
	{
		return tr->getPosition() + halfSize;
	}

	glm::vec2 min() const
	{
		return tr->getPosition() - halfSize;
	}

	glm::vec2 size() const
	{
		return halfSize * 2.0f;
	}

	// From VertexSource
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

	// From Shape
	bool isOverlaps(Shape* shape)
	{
		AABB* box = dynamic_cast<AABB*>(shape);
		if (box != nullptr)
		{
			return isOverlaps(*this, *box);
		}
		return false;
	}

	Collision getCollision(Shape* shape)
	{
		AABB* box = dynamic_cast<AABB*>(shape);
		if (box != nullptr)
		{
			return getShortestOverlap(*this, *box);
		}

		return Collision{};
	}

	static bool isOverlaps(AABB& box1, AABB& box2)
	{
		if (box2.min().x >= box1.max().x || box2.max().x <= box1.min().x) return false;
		if (box2.min().y >= box1.max().y || box2.max().y <= box1.min().y) return false;
		return true;
	}

	static Collision getShortestOverlap(AABB& box1, AABB& box2)
	{
		auto col = Collision{ true };

		// Resulting vector moves box1
		float x_overlap = getSegmentOverlap(box1.min().x, box1.max().x, box2.min().x, box2.max().x);
		float y_overlap = getSegmentOverlap(box1.min().y, box1.max().y, box2.min().y, box2.max().y);

		if (glm::abs(x_overlap) < glm::abs(y_overlap))
		{
			col.normal = glm::vec2(1, 0);
			col.depth = x_overlap;
		}
		else
		{
			col.normal = glm::vec2(0, 1);
			col.depth = y_overlap;
		}
		return col;
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