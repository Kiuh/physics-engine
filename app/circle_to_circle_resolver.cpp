#pragma once

#include "circle.cpp"
#include "collision.cpp"
#include "collision_resolver.cpp"

class CircleToCircleResolver : public CollisionResolver
{
	Circle* circle1;
	Circle* circle2;

	public:
	CircleToCircleResolver(Circle* circle1, Circle* circle2)
	{
		this->circle1 = circle1;
		this->circle2 = circle2;
	}

	bool isOverlaps()
	{
		auto dis = glm::distance(circle1->transform.pos(), circle2->transform.pos());
		return dis <= circle1->radius + circle2->radius;
	}

	Collision getForwardCollision()
	{
		auto overlapVec = overlap();
		return Collision{
			Contact{
				glm::vec2{0,0},
				glm::normalize(overlapVec),
				glm::length(overlapVec),
			}
		};
	}

	Collision getReverseCollision()
	{
		auto overlapVec = overlap();
		return Collision{
			Contact{
				glm::vec2{0,0},
				-glm::normalize(overlapVec),
				glm::length(overlapVec),
			}
		};
	}

	glm::vec2 overlap()
	{
		auto vec12 = circle1->transform.pos() - circle2->transform.pos();
		auto dist = glm::length(vec12);

		auto extra = (dist - circle1->radius) + (dist - circle2->radius);
		return vec12 - (glm::normalize(vec12) * extra);
	}
};