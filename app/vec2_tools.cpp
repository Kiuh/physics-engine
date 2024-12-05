#include "vec2_tools.h"

namespace vt
{
	void rotateVec2(glm::vec2& v, float angle)
	{
		float rad = glm::radians(angle);
		glm::mat2 rotationMatrix = glm::mat2(
			glm::cos(rad), -glm::sin(rad),
			glm::sin(rad), glm::cos(rad)
		);
		v = rotationMatrix * v;
	}

	glm::vec2 triple_product(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
		glm::vec3 A(a.x, a.y, 0);
		glm::vec3 B(b.x, b.y, 0);
		glm::vec3 C(c.x, c.y, 0);

		glm::vec3 first = glm::cross(A, B);
		glm::vec3 second = glm::cross(first, C);

		return glm::vec2(second.x, second.y);
	}

	SupportResult getSupport(const std::vector<glm::vec2>& dots, const glm::vec2& dir)
	{
		float furthest_distance = -std::numeric_limits<float>::infinity();
		glm::vec2 furthest_point{};
		size_t ind{};

		for (size_t i = 0; i < dots.size(); i++)
		{
			float distance = glm::dot(dots[i], dir);
			if (distance > furthest_distance)
			{
				furthest_distance = distance;
				furthest_point = dots[i];
				ind = i;
			}
		}

		return { ind, furthest_point };
	}

	glm::vec3 calcBarycentricCoords(const glm::vec2& P, const glm::vec2& A, const glm::vec2& B, const glm::vec2& C)
	{
		auto u0 = B - A;
		auto u1 = C - A;
		auto u2 = P - A;

		auto d00 = glm::dot(u0, u0);
		auto d01 = glm::dot(u0, u1);
		auto d11 = glm::dot(u1, u1);
		auto d20 = glm::dot(u2, u0);
		auto d21 = glm::dot(u2, u1);

		auto denominator = d00 * d11 - d01 * d01;

		auto beta = (d11 * d20 - d01 * d21) / denominator;
		auto gamma = (d00 * d21 - d01 * d20) / denominator;

		return {
			1 - (beta + gamma),
			beta,
			gamma
		};
	}
}
