#include "circle_circle_resolver.h"

bool _isCollide(const Circle& c1, const Circle& c2)
{
	auto dis = glm::distance(c1.tr.pos(), c2.tr.pos());
	return dis <= c1.radius + c2.radius;
}

Collision _getCollision(const Circle& c1, const Circle& c2)
{
	auto vec12 = c1.tr.pos() - c2.tr.pos();
	auto dist = glm::length(vec12);

	auto extra = (dist - c1.radius) + (dist - c2.radius);
	auto overlapVec = vec12 - (glm::normalize(vec12) * extra);

	return Collision{
		Contact{
			glm::vec2{0,0},
			glm::normalize(overlapVec),
			glm::length(overlapVec),
		}
	};
}