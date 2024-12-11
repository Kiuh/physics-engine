#pragma once

#include "collision.h"
#include "shape.h"
#include "transform_tools.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <stdexcept>

enum PolygonWinding
{
	Clockwise,
	CounterClockwise
};

struct Edge
{
	float distance;
	glm::vec2 normal;
	size_t index;
};

struct SimplexPoint
{
	glm::vec2 mink;
	size_t shAInd;
	size_t shBInd;
};

struct ContactData
{
	glm::vec2 localContactPointA;
	glm::vec2 localContactPointB;

	SimplexPoint A;
	SimplexPoint B;
	SimplexPoint C;
};

inline constexpr int MAX_INTERSECTION_ITERATIONS = 32;

class Collision2D
{
	private:
	std::vector<SimplexPoint> simplex_points{};
	glm::vec2 direction{};
	Shape& shapeA;
	Shape& shapeB;

	public:
	Collision2D(Shape& shapeA, Shape& shapeB);

	private:
	SimplexPoint getMinkSupport(const glm::vec2& dir);
	bool GJK();
	Edge findClosestEdge(PolygonWinding winding);
	Edge EPA();
	ContactData findContacts(Edge edge);

	public:
	std::optional<Collision> collide();

	static std::optional<Collision> tryCollide(Shape& sh1, Shape& sh2);
	static bool isOverlaps(Shape& sh1, Shape& sh2);
};