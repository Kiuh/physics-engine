#pragma once

#include <stdexcept>
#include "collision.cpp"

class CollisionResolver
{
	public:
	virtual bool isOverlaps() = 0;
	virtual Collision getForwardCollision() = 0;
	virtual Collision getReverseCollision() = 0;
};