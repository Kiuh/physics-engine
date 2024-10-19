#pragma once

#include "circle.h"
#include "box.hpp"
#include "collision.hpp"
#include "collision_resolver.cpp"
#include "box_circle_tools.cpp"

class CircleToBoxResolver : public CollisionResolver
{
	Circle* circle;
	Box* box;

	public:
	CircleToBoxResolver(Circle* circle, Box* box)
	{
		this->circle = circle;
		this->box = box;
	}

	bool isOverlaps()
	{
		return isTouchBoxCircle(*box, *circle);
	}

	Collision getForwardCollision()
	{
		auto result = getBoxCircleCollision(*box, *circle);
		result.contact.normal *= -1;
		return result;
	}

	Collision getReverseCollision()
	{
		return getBoxCircleCollision(*box, *circle);
	}
};