#pragma once

#include "glm/glm.hpp"
#include "math_tools.cpp"
#include "box.cpp"
#include "circle.cpp"
#include "collision.cpp"

static bool isTouchBoxCircle(Box* box, Circle* circle)
{
	auto dir_vec = circle->transform->getPos() - box->transform->getPos();
	auto len = glm::length(dir_vec);
	auto deg = glm::degrees(dir_vec);

	auto rad_vec = glm::vec2(circle->radius, circle->radius);
	auto circle_min = circle->transform->getPos() - rad_vec;
	auto circle_max = circle->transform->getPos() + rad_vec;

	if (circle_min.x >= box->max().x || circle_max.x <= box->min().x) return false;
	if (circle_min.y >= box->max().y || circle_max.y <= box->min().y) return false;
	return true;
}

static Collision getBoxCircleCollision(Box* box, Circle* circle)
{
	auto vec = box->transform->getPos() - circle->transform->getPos();
	return Collision{
		glm::normalize(vec),
		glm::length(vec)
	};
}