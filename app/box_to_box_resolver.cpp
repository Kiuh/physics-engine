#pragma once

#include "aabb.cpp"
#include "collision.cpp"
#include "collision_resolver.cpp"

class BoxToBoxResolver : public CollisionResolver
{
	AABB* box1;
	AABB* box2;

	public:
	BoxToBoxResolver(AABB* box1, AABB* box2)
	{
		this->box1 = box1;
		this->box2 = box2;
	}

	bool isOverlaps()
	{
		if (box2->min().x >= box1->max().x || box2->max().x <= box1->min().x) return false;
		if (box2->min().y >= box1->max().y || box2->max().y <= box1->min().y) return false;
		return true;
	}

	Collision getForwardCollision()
	{
		return getCollision(box1, box2);
	}

	Collision getReverseCollision()
	{
		return getCollision(box2, box1);
	}

	static Collision getCollision(AABB* box1, AABB* box2)
	{
		auto col = Collision{};

		// Resulting vector moves box1
		float x_overlap = getSegmentOverlap(box1->min().x, box1->max().x, box2->min().x, box2->max().x);
		float y_overlap = getSegmentOverlap(box1->min().y, box1->max().y, box2->min().y, box2->max().y);

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