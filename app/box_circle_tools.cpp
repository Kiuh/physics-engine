#include "box_circle_tools.h"

BoxCircleTouchParams getParams(const Box& box, const Circle& circle)
{
	auto dir_vec = circle.tr.pos() - box.tr.pos();
	auto deg = glm::degrees(glm::orientedAngle(glm::vec2(1.0f, 0), glm::normalize(dir_vec)));

	auto solve = [&circle, &box, dir_vec](Corner start, Corner end) -> BoxCircleTouchParams
		{
			auto l1 = mt::Line(circle.tr.pos(), box.tr.pos());
			auto l2 = mt::Line(box.getPos(start), box.getPos(end));

			auto p = intersection(l1, l2);
			auto boxPosToPoint = glm::distance(box.tr.pos(), p.value());

			return {
				boxPosToPoint,
				glm::length(dir_vec)
			};
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

bool isTouchBoxCircle(const Box& box, const Circle& circle)
{
	auto params = getParams(box, circle);
	return params.boxSide + circle.radius >= params.distance;
}

Collision getBoxCircleCollision(const Box& box, const Circle& circle)
{
	auto vec = box.tr.pos() - circle.tr.pos();
	auto params = getParams(box, circle);
	glm::vec2 norm{ 0,0 };
	if (fabs(vec.x) > fabs(vec.y))
	{
		norm.y = vec.y > 0.0f ? 1.0f : -1.0f;
	}
	else
	{
		norm.x = vec.x > 0.0f ? 1.0f : -1.0f;
	}
	return Collision{
			glm::vec2{0,0},
			norm,
			params.boxSide + circle.radius - params.distance,
	};
}