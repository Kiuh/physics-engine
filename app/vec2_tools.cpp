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


	bool isParallel(glm::vec2 vec1, glm::vec2 vec2)
	{
		auto angle = glm::degrees(glm::angle(vec1, vec2));
		return mt::is0_f(angle) || mt::is0_f(angle - 180.0f);
	}

	glm::vec2 rotate90(glm::vec2 vec)
	{
		auto x = vec.x;
		vec.x = -vec.y;
		vec.y = x;
		return vec;
	}

	bool in_range(glm::vec2 val, glm::vec2 a, glm::vec2 b)
	{
		return mt::in_range(val.x, a.x, b.x) && mt::in_range(val.y, a.y, b.y);
	}

	bool greater(glm::vec2 source, glm::vec2 comparison)
	{
		return source.x >= comparison.x && source.y >= comparison.y;
	}

	bool lesser(glm::vec2 source, glm::vec2 comparison)
	{
		return source.x <= comparison.x && source.y <= comparison.y;
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

	glm::vec2 getSupport(std::vector<glm::vec2> dots, glm::vec2 dir)
	{
		float furthest_distance = -std::numeric_limits<float>::infinity();
		glm::vec2 furthest_point{};

		for (glm::vec2& v : dots)
		{
			float distance = glm::dot(v, dir);
			if (distance > furthest_distance)
			{
				furthest_distance = distance;
				furthest_point = v;
			}
		}

		return furthest_point;
	}
}
