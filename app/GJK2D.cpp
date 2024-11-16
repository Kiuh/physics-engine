#include "GJK2D.h"

GJK2D::GJK2D() : shapeA(nullptr), shapeB(nullptr) {}

glm::vec2 GJK2D::getSupport(Shape* shape, glm::vec2 dir)
{
	float furthestDistance = -std::numeric_limits<float>::infinity();
	glm::vec2 furthestVertex{};

	for (glm::vec2& v : shape->getWorldPoints())
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

bool GJK2D::addSupport(const glm::vec2& dir)
{
	glm::vec2 newVertex = getSupport(shapeA, dir) - getSupport(shapeB, -dir);
	vertices.push_back(newVertex);
	return glm::dot(dir, newVertex) > 0;
}

glm::vec2 GJK2D::tripleProduct(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
	glm::vec3 A(a.x, a.y, 0);
	glm::vec3 B(b.x, b.y, 0);
	glm::vec3 C(c.x, c.y, 0);

	glm::vec3 first = glm::cross(A, B);
	glm::vec3 second = glm::cross(first, C);

	return glm::vec2(second.x, second.y);
}

EvolveResult GJK2D::evolveSimplex()
{
	switch (vertices.size())
	{
		case 0:
			direction = shapeB->tr.pos() - shapeA->tr.pos();
			break;
		case 1:
			direction *= -1;
			break;
		case 2:
			{
				glm::vec2 b = vertices[1];
				glm::vec2 c = vertices[0];

				glm::vec2 cb = b - c;
				glm::vec2 c0 = -c;

				direction = tripleProduct(cb, c0, cb);
				break;
			}
		case 3:
			{
				glm::vec2 a = vertices[2];
				glm::vec2 b = vertices[1];
				glm::vec2 c = vertices[0];

				glm::vec2 a0 = -a;
				glm::vec2 ab = b - a;
				glm::vec2 ac = c - a;

				glm::vec2 abPerp = tripleProduct(ac, ab, ab);
				glm::vec2 acPerp = tripleProduct(ab, ac, ac);

				if (glm::dot(abPerp, a0) > 0)
				{
					vertices.erase(vertices.begin() + 2); // Remove vertex c
					direction = abPerp;
				}
				else if (glm::dot(acPerp, a0) > 0)
				{
					vertices.erase(vertices.begin() + 1); // Remove vertex b
					direction = acPerp;
				}
				else
				{
					return FoundIntersection;
				}
				break;
			}
		default:
			throw std::runtime_error("Can't have simplex with more than 3 vertices!");
	}

	return addSupport(direction) ? StillEvolving : NoIntersection;
}

bool GJK2D::test(Shape* shapeA, Shape* shapeB)
{
	this->vertices.clear();
	this->shapeA = shapeA;
	this->shapeB = shapeB;

	EvolveResult result = StillEvolving;
	while (result == StillEvolving)
	{
		result = evolveSimplex();
	}

	return result == FoundIntersection;
}
