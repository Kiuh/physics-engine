#include "collision_tools.h"

bool isOverlaps(Shape& sh1, Shape& sh2)
{
	auto solver = std::make_unique<Collision2D>();
	return solver->GJK(&sh1, &sh2);
}

std::optional<Collision> getCollision(Shape& sh1, Shape& sh2)
{
	auto solver = std::make_unique<Collision2D>();
	return solver->collide(&sh1, &sh2);
}

std::optional<std::vector<glm::vec2>> getWorldContactPoints(Shape& sh1, Shape& sh2)
{
	auto solver = std::make_unique<Collision2D>();
	return solver->EPA(&sh1, &sh2);
}
