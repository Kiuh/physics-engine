#pragma once

#include "shape.h"
#include "collision.h"
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

class GJK2D
{
	private:
	std::vector<glm::vec2> simplex_points{};
	glm::vec2 direction{};
	Shape* shapeA;
	Shape* shapeB;

	public:
	GJK2D();

	private:
	bool addSupport(const glm::vec2& dir);
	Edge findClosestEdge(PolygonWinding winding);

	public:
	bool test(Shape* shapeA, Shape* shapeB);
	std::optional<Collision> intersect(Shape* shapeA, Shape* shapeB);
};