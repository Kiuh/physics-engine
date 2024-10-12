#pragma once

#include "glm/glm.hpp"
#include "aabb.cpp"
#include "circle.cpp"
#include "collision.cpp"

static bool isTouchBoxCircle(AABB* box, Circle* circle)
{
	auto rad_vec = glm::vec2(circle->radius, circle->radius);
	auto circle_min = circle->transform->getPos() - rad_vec;
	auto circle_max = circle->transform->getPos() + rad_vec;

	if (circle_min.x >= box->max().x || circle_max.x <= box->min().x) return false;
	if (circle_min.y >= box->max().y || circle_max.y <= box->min().y) return false;
	return true;
}

static Collision getBoxCircleCollision(AABB* box, Circle* circle)
{
	auto vec = box->transform->getPos() - circle->transform->getPos();
	return Collision{
		glm::normalize(vec),
		glm::length(vec)
	};
}