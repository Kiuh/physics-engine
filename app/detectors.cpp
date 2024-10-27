#include "detectors.h"

#include "box_circle_tools.h"

bool detectCollision(const Shape& sh1, const Shape& sh2)
{
	const Box* box1 = dynamic_cast<const Box*>(&sh1);
	const Box* box2 = dynamic_cast<const Box*>(&sh2);

	if (box1 && box2) return detectCollisionBB(*box1, *box2);

	const Circle* circle1 = dynamic_cast<const Circle*>(&sh1);
	const Circle* circle2 = dynamic_cast<const Circle*>(&sh2);

	if (circle1 && circle2) return detectCollisionCC(*circle1, *circle2);

	if (box1 && circle2) return detectCollisionBC(*box1, *circle2);
	if (circle1 && box2) return detectCollisionBC(*box2, *circle1);

	throw std::runtime_error("Unhandled shapes combination!");
}

bool detectCollisionBB(const Box& box1, const Box& box2)
{
	if (box2.min().x >= box1.max().x || box2.max().x <= box1.min().x) return false;
	if (box2.min().y >= box1.max().y || box2.max().y <= box1.min().y) return false;
	return true;
}

bool detectCollisionCC(const Circle& c1, const Circle& c2)
{
	auto dis = glm::distance(c1.tr.pos(), c2.tr.pos());
	return dis <= c1.radius + c2.radius;
}

bool detectCollisionBC(const Box& box, const Circle& circle)
{
	auto inters = getIntersectionsPoints(box, circle);
	size_t points = 0;
	for (auto& inter : inters)
	{
		points += inter.points.size();
	}
	return points == 1 || points == 2;
}