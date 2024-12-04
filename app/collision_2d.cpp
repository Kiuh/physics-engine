#include "collision_2d.h"

Collision2D::Collision2D(Shape& shapeA, Shape& shapeB) : shapeA(shapeA), shapeB(shapeB) {}

// Add support point of minkovski difference to simplex
// Returns true if new point and given direction in the same direction
SimplexPoint Collision2D::getMinkSupport(const glm::vec2& dir)
{
	auto sup_a = vt::getSupport(shapeA.getWorldPoints(), dir);
	auto sup_b = vt::getSupport(shapeB.getWorldPoints(), -dir);
	return {
		.mink = sup_a.second - sup_b.second,
		.shAInd = sup_a.first,
		.shBInd = sup_b.first,
	};
}

bool Collision2D::GJK()
{
	this->simplex_points.clear();

	// Create first simplex
	direction = shapeB.tr.pos() - shapeA.tr.pos();
	simplex_points.push_back(getMinkSupport(direction)); // From direction of colliding

	direction *= -1;
	simplex_points.push_back(getMinkSupport(direction)); // Opposite of direction of colliding

	glm::vec2 b = simplex_points[1].mink;
	glm::vec2 c = simplex_points[0].mink;

	glm::vec2 cb = b - c;
	glm::vec2 c0 = -c;
	direction = vt::triple_product(cb, c0, cb);
	simplex_points.push_back(getMinkSupport(direction)); // Perpendicular to 0 -> 1 line facing origin

	// Test and try new simplexes while they exist
	do
	{
		glm::vec2 a = simplex_points[2].mink;
		glm::vec2 b = simplex_points[1].mink;
		glm::vec2 c = simplex_points[0].mink;

		glm::vec2 a0 = -a;
		glm::vec2 ab = b - a;
		glm::vec2 ac = c - a;

		glm::vec2 abPerp = vt::triple_product(ac, ab, ab);
		glm::vec2 acPerp = vt::triple_product(ab, ac, ac);

		if (glm::dot(abPerp, a0) > 0)
		{
			simplex_points.erase(simplex_points.begin()); // Remove vertex c
			direction = abPerp;
		}
		else if (glm::dot(acPerp, a0) > 0)
		{
			simplex_points.erase(simplex_points.begin() + 1); // Remove vertex b
			direction = acPerp;
		}
		else // Origin inside simplex
		{
			return true;
		}

		simplex_points.push_back(getMinkSupport(direction));
	}
	while (glm::dot(direction, simplex_points.back().mink) > 0);
	return false;
}


Edge Collision2D::findClosestEdge(PolygonWinding winding)
{
	float closestDistance = std::numeric_limits<float>::infinity();
	glm::vec2 closestNormal{};
	size_t closestIndex = 0;

	for (size_t i = 0; i < simplex_points.size(); i++)
	{
		size_t j = i + 1;
		if (j >= simplex_points.size())
		{
			j = 0;
		}

		glm::vec2 line = simplex_points[j].mink - simplex_points[i].mink;

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
		float dist = glm::dot(norm, simplex_points[i].mink);
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestNormal = norm;
			closestIndex = j;
		}
	}

	return Edge{ closestDistance, closestNormal, closestIndex };
}

Edge Collision2D::EPA()
{
	float e0 = (simplex_points[1].mink.x - simplex_points[0].mink.x) * (simplex_points[1].mink.y + simplex_points[0].mink.y);
	float e1 = (simplex_points[2].mink.x - simplex_points[1].mink.x) * (simplex_points[2].mink.y + simplex_points[1].mink.y);
	float e2 = (simplex_points[0].mink.x - simplex_points[2].mink.x) * (simplex_points[0].mink.y + simplex_points[2].mink.y);
	PolygonWinding winding = (e0 + e1 + e2 >= 0) ? PolygonWinding::Clockwise : PolygonWinding::CounterClockwise;

	Edge edge{};
	for (size_t i = 0; i < MAX_INTERSECTION_ITERATIONS; i++)
	{
		edge = findClosestEdge(winding);

		auto support = getMinkSupport(edge.normal);
		auto distance = glm::dot(support.mink, edge.normal);

		if (!mt::is0_f(distance - edge.distance))
		{
			simplex_points.insert(simplex_points.begin() + edge.index, support);
		}
		else
		{
			break;
		}
	}

	return edge;
}

ContactData Collision2D::findContacts(Edge edge)
{
	auto shA_points = shapeA.getWorldPoints();
	auto shB_points = shapeB.getWorldPoints();

	auto& A = simplex_points[edge.index];
	auto& B = simplex_points[(edge.index + 1) % simplex_points.size()];
	auto& C = simplex_points[(edge.index + 2) % simplex_points.size()];
	auto P = edge.normal * edge.distance;

	auto bc = vt::calcBarycentricCoords(P, A.mink, B.mink, C.mink);

	return ContactData{
		.localContactPointA =
			bc.x * shA_points[A.shAInd] +
			bc.y * shA_points[B.shAInd] +
			bc.z * shA_points[C.shAInd],
		.localContactPointB =
			bc.x * shB_points[A.shBInd] +
			bc.y * shB_points[B.shBInd] +
			bc.z * shB_points[C.shBInd],
	};
}

std::optional<Collision> Collision2D::collide()
{
	if (!GJK())
	{
		return {};
	}

	auto edge = EPA();
	auto contacts = findContacts(edge);

	return {
		Collision{
			.normalAB = -edge.normal,
			.normalBA = edge.normal,
			.penetration = edge.distance,
			.localContactPointA = contacts.localContactPointA,
			.localContactPointB = contacts.localContactPointB,
			.worldContactPointA = contacts.localContactPointA + shapeA.tr.pos(),
			.worldContactPointB = contacts.localContactPointB + shapeB.tr.pos(),
		}
	};
}

std::optional<Collision> Collision2D::tryCollide(Shape& sh1, Shape& sh2)
{
	auto solver = std::make_unique<Collision2D>(sh1, sh2);
	return solver->collide();
}

bool Collision2D::isOverlaps(Shape& sh1, Shape& sh2)
{
	auto solver = std::make_unique<Collision2D>(sh1, sh2);
	return solver->GJK();
}
