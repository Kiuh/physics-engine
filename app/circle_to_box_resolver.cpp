#pragma once

#include "circle.cpp"
#include "aabb.cpp"
#include "collision.cpp"
#include "collision_resolver.cpp"
#include "box_circle_tools.cpp"

class CircleToBoxResolver : public CollisionResolver
{
	Circle* circle;
	AABB* box;

	public:
	CircleToBoxResolver(Circle* circle, AABB* box)
	{
		this->circle = circle;
		this->box = box;
	}

	bool isOverlaps()
	{
		return isTouchBoxCircle(box, circle);
	}

	Collision getForwardCollision()
	{
		auto result = getBoxCircleCollision(box, circle);
		result.normal *= -1;
		return result;
	}

	Collision getReverseCollision()
	{
		return getBoxCircleCollision(box, circle);
	}
};