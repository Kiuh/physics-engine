#pragma once

#include "shape.cpp"
#include "collision_resolver.cpp"

#include "box_to_box_resolver.cpp"
#include "circle_to_circle_resolver.cpp"
#include "box_to_circle_resolver.cpp"
#include "circle_to_box_resolver.cpp"

static CollisionResolver* createCollisionResolver(Shape* sh1, Shape* sh2)
{
	Box* box1 = dynamic_cast<Box*>(sh1);
	Box* box2 = dynamic_cast<Box*>(sh2);

	if (box1 != nullptr && box2 != nullptr)
	{
		return new BoxToBoxResolver(box1, box2);
	}

	Circle* circle1 = dynamic_cast<Circle*>(sh1);
	Circle* circle2 = dynamic_cast<Circle*>(sh2);

	if (circle1 != nullptr && circle2 != nullptr)
	{
		return new CircleToCircleResolver(circle1, circle2);
	}

	if (box1 != nullptr && circle2 != nullptr)
	{
		return new BoxToCircleResolver(box1, circle2);
	}

	if (circle1 != nullptr && box2 != nullptr)
	{
		return new CircleToBoxResolver(circle1, box2);
	}

	throw std::runtime_error("Unhandled shapes combination!");
}