#include "collision_tools.h"

bool isOverlaps(Shape& sh1, Shape& sh2)
{
	auto gjk = std::make_unique<GJK2D>();
	return gjk->test(&sh1, &sh2);
}

std::optional<Collision> intersection(Shape& sh1, Shape& sh2)
{
	auto gjk = std::make_unique<GJK2D>();
	return gjk->intersect(&sh1, &sh2);
}