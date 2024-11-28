#pragma once

#include "collision.h"
#include "shape.h"
#include <memory>
#include <optional>
#include <stdexcept>

enum PolygonWinding
{
	Clockwise,
	CounterClockwise
};

enum EvolveResult
{
	NoIntersection,
	FoundIntersection,
	StillEvolving
};

struct Edge
{
	float distance;
	glm::vec2 normal;
	int index;
};

inline constexpr int MAX_INTERSECTION_ITERATIONS = 32;

class Collision2D
{
	private:
	std::vector<glm::vec2> simplex_points{};
	glm::vec2 direction{};
	Shape* shapeA;
	Shape* shapeB;

	public:
	Collision2D();

	private:
	bool addSupport(const glm::vec2& dir);
	Edge findClosestEdge(PolygonWinding winding);
	Edge EPA(Shape* shapeA, Shape* shapeB);
	bool GJK(Shape* shapeA, Shape* shapeB);

	public:
	std::optional<Collision> collide(Shape* shapeA, Shape* shapeB);

	static std::optional<Collision> tryCollide(Shape& sh1, Shape& sh2);
	static bool isOverlaps(Shape& sh1, Shape& sh2);
};