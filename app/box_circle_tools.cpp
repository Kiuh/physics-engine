#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/glm.hpp"
#include "math_tools.cpp"
#include "box.cpp"
#include "circle.cpp"
#include "collision.cpp"
#include <glm/gtx/vector_angle.hpp>
#include <iostream>

static bool isTouchBoxCircle(Box& box, Circle& circle)
{
	auto dir_vec = circle.transform.pos() - box.transform.pos();
	auto deg = glm::degrees(glm::orientedAngle(glm::vec2(1.0f, 0), glm::normalize(dir_vec)));

	auto solve = [circle, box, dir_vec](Corner start, Corner end) -> bool
		{
			auto l1 = Line(circle.transform.pos(), box.transform.pos());
			auto l2 = Line(box.getPos(start), box.getPos(end));

			auto p = intersection(l1, l2);
			auto a = glm::distance(box.transform.pos(), p.value());

			return a + circle.radius >= glm::length(dir_vec);
		};

	//  (135) * - * (45) up   (+)
	//        |   |--- O
	// (-135) * - * (-45) down (-)
	auto r_u = box.getDegrees(RightUp);
	auto r_d = box.getDegrees(RightDown);
	auto l_u = box.getDegrees(LeftUp);
	auto l_d = box.getDegrees(LeftDown);

	if (deg <= r_u && deg >= r_d)
	{
		// Right
		return solve(RightUp, RightDown);
	}

	if (deg <= l_u && deg >= r_u)
	{
		// Up
		return solve(RightUp, LeftUp);
	}

	if (deg <= r_d && deg >= l_d)
	{
		// Down
		return solve(LeftDown, RightDown);
	}

	// Left
	return solve(LeftUp, LeftDown);
}

static Collision getBoxCircleCollision(Box& box, Circle& circle)
{
	auto vec = box.transform.pos() - circle.transform.pos();
	return Collision{
		Contact{
			glm::vec2{0,0},
			glm::normalize(vec),
			glm::length(vec),
		}
	};
}