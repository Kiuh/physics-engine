#include "vec2_tools.h"

namespace vt
{
	float cross_vec2(const glm::vec2& o, const glm::vec2& a, const glm::vec2& b)
	{
		return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
	}

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
		glm::vec2 ab = B - A;
		glm::vec2 ac = C - A;
		glm::vec2 ap = P - A;

		float d1 = glm::dot(ab, ap);
		float d2 = glm::dot(ac, ap);

		if (d1 <= 0.0f && d2 <= 0.0f)
		{
			return glm::vec3(1.0f, 0.0f, 0.0f); // Closest to A
		}

		glm::vec2 bp = P - B;
		float d3 = glm::dot(ab, bp);
		float d4 = glm::dot(ac, bp);

		if (d3 >= 0.0f && d4 <= d3)
		{
			return glm::vec3(0.0f, 1.0f, 0.0f); // Closest to B
		}

		float vc = d1 * d4 - d3 * d2;
		if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
		{
			float v = d1 / (d1 - d3);
			return glm::vec3(1.0f - v, v, 0.0f); // On AB edge
		}

		glm::vec2 cp = P - C;
		float d5 = glm::dot(ab, cp);
		float d6 = glm::dot(ac, cp);

		if (d6 >= 0.0f && d5 <= d6)
		{
			return glm::vec3(0.0f, 0.0f, 1.0f); // Closest to C
		}

		float vb = d5 * d2 - d1 * d6;
		if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
		{
			float w = d2 / (d2 - d6);
			return glm::vec3(1.0f - w, 0.0f, w); // On AC edge
		}

		float va = d3 * d6 - d5 * d4;
		if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
		{
			float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			return glm::vec3(0.0f, 1.0f - w, w); // On BC edge
		}

		float d = 1.0f / (va + vb + vc);
		float v = vb * d;
		float w = vc * d;

		return glm::vec3(1.0f - v - w, v, w); // Inside triangle
	}

	std::vector<glm::vec2> createConvexHull(const std::vector<glm::vec2>& _points)
	{
		std::vector<glm::vec2> points = _points;
		std::sort(
			points.begin(),
			points.end(),
			[](const glm::vec2& a, const glm::vec2& b)
			{
				return a.x < b.x || (a.x == b.x && a.y < b.y);
			}
		);

		std::vector<glm::vec2> lower, upper;

		// Build the lower hull
		for (const auto& p : points)
		{
			while (lower.size() >= 2 && vt::cross_vec2(lower[lower.size() - 2], lower[lower.size() - 1], p) <= 0)
			{
				lower.pop_back();
			}
			lower.push_back(p);
		}

		// Build the upper hull
		for (int i = points.size() - 1; i >= 0; i--)
		{
			while (upper.size() >= 2 && vt::cross_vec2(upper[upper.size() - 2], upper[upper.size() - 1], points[i]) <= 0)
			{
				upper.pop_back();
			}
			upper.push_back(points[i]);
		}

		// Remove the last point of each half because it's repeated at the beginning of the other half
		lower.pop_back();
		upper.pop_back();

		// Concatenate lower and upper hull to form the full convex hull
		lower.insert(lower.end(), upper.begin(), upper.end());
		return lower;
	}
}
