#pragma once

#include "circle.h"
#include "box.hpp"
#include "collision.hpp"
#include "collision_resolver.cpp"
#include "box_circle_tools.cpp"

class BoxToCircleResolver : public CollisionResolver
{
	Box& box;
	Circle& circle;

	public:
	BoxToCircleResolver(Box& box, Circle& circle) : box(box), circle(circle)
	{
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
		result.contact.normal *= -1;
		return result;
	}
};