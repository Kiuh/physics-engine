#include "resolvers.h"

Collision createCollision(const Shape& sh1, const Shape& sh2)
{
	const Box* box1 = dynamic_cast<const Box*>(&sh1);
	const Box* box2 = dynamic_cast<const Box*>(&sh2);

	if (box1 != nullptr && box2 != nullptr)
	{
		return createCollisionBB(*box1, *box2);
	}

	const Circle* circle1 = dynamic_cast<const Circle*>(&sh1);
	const Circle* circle2 = dynamic_cast<const Circle*>(&sh2);

	if (circle1 != nullptr && circle2 != nullptr)
	{
		return createCollisionCC(*circle1, *circle2);
	}

	if (box1 != nullptr && circle2 != nullptr)
	{
		return createCollisionBC(*box1, *circle2);
	}

	if (circle1 != nullptr && box2 != nullptr)
	{
		return createCollisionCB(*circle1, *box2);
	}

	throw std::runtime_error("Unhandled shapes combination!");
}

Collision createCollisionBB(const Box& box1, const Box& box2)
{
	auto col = Collision{};

	// Resulting vector moves box1
	float x_overlap = mt::getSegmentOverlap(box1.min().x, box1.max().x, box2.min().x, box2.max().x);
	float y_overlap = mt::getSegmentOverlap(box1.min().y, box1.max().y, box2.min().y, box2.max().y);

	if (glm::abs(x_overlap) < glm::abs(y_overlap))
	{
		col.contact.normal = glm::vec2(1, 0);
		col.contact.penetration = x_overlap;
	}
	else
	{
		col.contact.normal = glm::vec2(0, 1);
		col.contact.penetration = y_overlap;
	}
	return col;
}

Collision createCollisionCC(const Circle& c1, const Circle& c2)
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

Collision createCollisionBC(const Box& b, const Circle& c)
{
	return getBoxCircleCollision(b, c);
}

Collision createCollisionCB(const Circle& c, const Box& b)
{
	auto result = getBoxCircleCollision(b, c);
	result.contact.normal *= -1;
	return result;
}