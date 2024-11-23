#include "GJK2D.h"

Collision2D::Collision2D() : shapeA(nullptr), shapeB(nullptr) {}

static glm::vec2 getSupport(std::vector<glm::vec2> dots, glm::vec2 dir)
{
	float furthestDistance = -std::numeric_limits<float>::infinity();
	glm::vec2 furthestVertex{};

	for (glm::vec2& v : dots)
	{
		float distance = glm::dot(v, dir);
		if (distance > furthestDistance)
		{
			furthestDistance = distance;
			furthestVertex = v;
		}
	}

	return furthestVertex;
}

static glm::vec2 calculateSupport(Shape* shapeA, Shape* shapeB, const glm::vec2& direction) {
	glm::vec2 newVertex = getSupport(shapeA->getWorldPoints(), direction);
	newVertex -= getSupport(shapeB->getWorldPoints(), -direction);
	return newVertex;
}


bool Collision2D::addSupport(const glm::vec2& dir)
{
	auto sup_a = getSupport(shapeA->getWorldPoints(), dir);
	auto sup_b = getSupport(shapeB->getWorldPoints(), -dir);
	glm::vec2 newVertex = sup_a - sup_b;
	simplex_points.push_back(newVertex);
	return glm::dot(dir, newVertex) > 0;
}

Edge Collision2D::findClosestEdge(PolygonWinding winding)
{
	float closestDistance = std::numeric_limits<float>::infinity();
	glm::vec2 closestNormal{};
	int closestIndex = 0;
	glm::vec2 line;

	for (size_t i = 0; i < simplex_points.size(); ++i)
	{
		size_t j = i + 1;
		if (j >= simplex_points.size())
		{
			j = 0;
		}

		line = simplex_points[j] - simplex_points[i];

		glm::vec2 norm;
		if (winding == PolygonWinding::Clockwise)
		{
			norm = glm::vec2(line.y, -line.x);
		}
		else
		{
			norm = glm::vec2(-line.y, line.x);
		}
		norm = glm::normalize(norm);

		// Calculate how far the edge is from the origin
		float dist = glm::dot(norm, simplex_points[i]);
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestNormal = norm;
			closestIndex = static_cast<int>(j);
		}
	}

	return Edge{ closestDistance, closestNormal, closestIndex };
}

bool Collision2D::GJK(Shape* shapeA, Shape* shapeB)
{
	this->simplex_points.clear();
	this->shapeA = shapeA;
	this->shapeB = shapeB;

	// Create first simplex
	direction = shapeB->tr.pos() - shapeA->tr.pos();
	addSupport(direction);

	direction *= -1;
	addSupport(direction);

	glm::vec2 b = simplex_points[1];
	glm::vec2 c = simplex_points[0];

	glm::vec2 cb = b - c;
	glm::vec2 c0 = -c;
	direction = vt::triple_product(cb, c0, cb);
	addSupport(direction);

	// Try simplexes while they exist
	do
	{
		glm::vec2 a = simplex_points[2];
		glm::vec2 b = simplex_points[1];
		glm::vec2 c = simplex_points[0];

		glm::vec2 a0 = -a;
		glm::vec2 ab = b - a;
		glm::vec2 ac = c - a;

		glm::vec2 abPerp = vt::triple_product(ac, ab, ab);
		glm::vec2 acPerp = vt::triple_product(ab, ac, ac);

		if (glm::dot(abPerp, a0) > 0)
		{
			simplex_points.erase(simplex_points.begin() + 2); // Remove vertex c
			direction = abPerp;
		}
		else if (glm::dot(acPerp, a0) > 0)
		{
			simplex_points.erase(simplex_points.begin() + 1); // Remove vertex b
			direction = acPerp;
		}
		else
		{
			return true;
		}
	}
	while (addSupport(direction));
	return false;
}

std::optional<Collision> Collision2D::collide(Shape* shapeA, Shape* shapeB)
{
	if (!GJK(shapeA, shapeB))
	{
		return {};
	}

	float e0 = (simplex_points[1].x - simplex_points[0].x) * (simplex_points[1].y + simplex_points[0].y);
	float e1 = (simplex_points[2].x - simplex_points[1].x) * (simplex_points[2].y + simplex_points[1].y);
	float e2 = (simplex_points[0].x - simplex_points[2].x) * (simplex_points[0].y + simplex_points[2].y);

	PolygonWinding winding = (e0 + e1 + e2 >= 0) ? PolygonWinding::Clockwise : PolygonWinding::CounterClockwise;

	Edge edge{};
	for (int i = 0; i < MAX_INTERSECTION_ITERATIONS; i++)
	{
		edge = findClosestEdge(winding);
		glm::vec2 support = calculateSupport(shapeA, shapeB, edge.normal);
		float distance = glm::dot(support, edge.normal);

		if (!mt::is0_f(distance - edge.distance))
		{
			simplex_points.insert(simplex_points.begin() + edge.index, support);
		}
		else
		{
			break;
		}
	}

	return {
		Collision{
			.normal = edge.normal,
			.penetration = edge.distance
		}
	};
}

std::optional<std::vector<glm::vec2>> Collision2D::EPA(Shape* shapeA, Shape* shapeB)
{
	if (!GJK(shapeA, shapeB))
	{
		return {};
	}

	std::vector<glm::vec2> result{};
	auto sh1p = shapeA->getWorldPoints();
	auto sh2p = shapeB->getWorldPoints();
	result.insert(result.begin(), sh1p.begin(), sh1p.end());
	result.insert(result.begin(), sh2p.begin(), sh2p.end());
	return { result };
}
