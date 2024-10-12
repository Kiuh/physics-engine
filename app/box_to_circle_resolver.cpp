#pragma once

#include "circle.cpp"
#include "aabb.cpp"
#include "collision.cpp"
#include "collision_resolver.cpp"
#include "box_circle_tools.cpp"

class BoxToCircleResolver : public CollisionResolver
{
	AABB* box;
	Circle* circle;

	public:
	BoxToCircleResolver(AABB* box, Circle* circle)
	{
		this->box = box;
		this->circle = circle;
	}

	bool isOverlaps()
	{
		return isTouchBoxCircle(box, circle);
	}

	Collision getForwardCollision()
	{
		return getBoxCircleCollision(box, circle);
	}

	Collision getReverseCollision()
	{
		auto result = getBoxCircleCollision(box, circle);
		result.normal *= -1;
		return result;
	}
};