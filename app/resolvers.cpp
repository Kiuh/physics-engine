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
	// Resulting vector moves box1
	float x_overlap = mt::getSegmentOverlap(box1.min().x, box1.max().x, box2.min().x, box2.max().x);
	float y_overlap = mt::getSegmentOverlap(box1.min().y, box1.max().y, box2.min().y, box2.max().y);

	if (glm::abs(x_overlap) < glm::abs(y_overlap))
	{
		return {
			glm::vec2(0,0),
			glm::vec2(mt::clamp_m11(1 * x_overlap), 0),
			glm::abs(x_overlap),
		};
	}

	return {
		glm::vec2(0,0),
		glm::vec2(0, mt::clamp_m11(1 * y_overlap)),
		glm::abs(y_overlap),
	};
}

Collision createCollisionCC(const Circle& c1, const Circle& c2)
{
	auto vec21 = c1.tr.pos() - c2.tr.pos();
	auto dist = glm::length(vec21);

	auto overlap = c1.radius + c2.radius - dist;
	auto norm = glm::normalize(vec21);

	return Collision{
		c1.tr.pos() + norm * c1.radius,
		norm,
		overlap,
	};
}

Collision createCollisionBC(const Box& b, const Circle& circle)
{
	auto inters = getIntersectionsPoints(b, circle);

	// If circle overlap with only one side of box
	if (inters.size() == 1)
	{
		auto& sec = inters[0];

		// And it have only 1 contact point
		if (sec.points.size() == 1)
		{
			return {
				sec.points[0],
				glm::normalize(sec.points[0] - circle.tr.pos()),
				0
			};
		}

		// And it have 2 contact points
		if (sec.points.size() == 2)
		{
			auto dot = (sec.points[0] + sec.points[1]) / 2.0f;
			glm::vec2 vec{};
			if (mt::intersection(sec.section, mt::Section(b.tr.pos(), circle.tr.pos())).has_value())
			{
				// Circle outside box
				vec = dot - circle.tr.pos();
			}
			else
			{
				// Circle inside box
				vec = circle.tr.pos() - dot;
			}
			return {
				dot,
				glm::normalize(vec),
				glm::length(vec)
			};
		}
	}

	if (inters.size() == 2)
	{
		return {};
	}

	throw new std::runtime_error("Cannot resolve collision in this situation.");
}

Collision createCollisionCB(const Circle& circle, const Box& b)
{
	auto inters = getIntersectionsPoints(b, circle);

	if (inters.size() == 1)
	{
		auto& sec = inters[0];

		// And it have only 1 contact point
		if (sec.points.size() == 1)
		{
			return {
				sec.points[0],
				glm::normalize(sec.points[0] - circle.tr.pos()),
				0
			};
		}

		return {};
	}

	if (inters.size() == 2)
	{
		return {};
	}

	throw new std::runtime_error("Cannot resolve collision in this situation.");
}