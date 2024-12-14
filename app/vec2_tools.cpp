#include "vec2_tools.h"

namespace vt
{
	float cross(const glm::vec2& a, const glm::vec2& b)
	{
		return a.x * b.y - b.x * a.y;
	}

	float cross(const glm::vec2& o, const glm::vec2& a, const glm::vec2& b)
	{
		return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
	}

	glm::vec2 rotateVec2(const glm::vec2& v, const float& angle)
	{
		float rad = glm::radians(angle);
		glm::mat2 rotationMatrix = glm::mat2(
			glm::cos(rad), -glm::sin(rad),
			glm::sin(rad), glm::cos(rad)
		);
		return rotationMatrix * v;
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

	static bool compareVec2(const glm::vec2& a, const glm::vec2& b)
	{
		return a.x < b.x || (a.x == b.x && a.y < b.y);
	}

	std::vector<glm::vec2> createConvexHull(const std::vector<glm::vec2>& points)
	{
		std::vector<glm::vec2> result = points;
		std::sort(result.begin(), result.end(), compareVec2);

		std::vector<glm::vec2> lower, upper;

		// Build the lower hull
		for (const auto& p : result)
		{
			while (lower.size() >= 2 && vt::cross(lower[lower.size() - 2], lower[lower.size() - 1], p) <= 0)
			{
				lower.pop_back();
			}
			lower.push_back(p);
		}

		// Build the upper hull
		for (int i = static_cast<int>(result.size()) - 1; i >= 0; i--)
		{
			while (upper.size() >= 2 && vt::cross(upper[upper.size() - 2], upper[upper.size() - 1], result[i]) <= 0)
			{
				upper.pop_back();
			}
			upper.push_back(result[i]);
		}

		// Remove the last point of each half because it's repeated at the beginning of the other half
		lower.pop_back();
		upper.pop_back();

		lower.insert(lower.end(), upper.begin(), upper.end());
		return lower;
	}

	float calcArea(const std::vector<glm::vec2>& points)
	{
		float area = 0.0f;
		for (size_t i = 1; i < points.size() - 1; i++)
		{
			auto v1 = points[i + 1] - points[0];
			auto v2 = points[i] - points[0];
			area += cross(v1, v2) / 2;
		}
		return fabs(area);
	}

	float calcMomentOfInertia(const glm::vec2& center, const std::vector<glm::vec2>& points, float density)
	{
		float momentOfInertia = 0.0f;

		for (size_t i = 0; i < points.size() - 1; ++i)
		{
			const glm::vec2& p2 = points[i];
			const glm::vec2& p3 = points[i + 1];

			float w = glm::distance(center, p2);
			float w1 = glm::abs(glm::dot(center - p2, p3 - p2) / w);
			float w2 = glm::abs(w - w1);

			float signedTriArea = cross(p3 - center, p2 - center) / 2.0f;
			float h = 2.0f * glm::abs(signedTriArea) / w;

			glm::vec2 p4 = p2 + (center - p2) * (w1 / w);
			glm::vec2 cm1 = (p2 + p3 + p4) / 3.0f;
			glm::vec2 cm2 = (center + p3 + p4) / 3.0f;

			float I1 = density * w1 * h * ((h * h / 4.0f) + (w1 * w1 / 12.0f));
			float I2 = density * w2 * h * ((h * h / 4.0f) + (w2 * w2 / 12.0f));

			float m1 = 0.5f * w1 * h * density;
			float m2 = 0.5f * w2 * h * density;

			float I1cm = I1 - (m1 * powf(glm::distance(cm1, p3), 2));
			float I2cm = I2 - (m2 * powf(glm::distance(cm2, p3), 2));

			float momentOfInertiaPart1 = I1cm + (m1 * powf(glm::length(cm1), 2));
			float momentOfInertiaPart2 = I2cm + (m2 * powf(glm::length(cm2), 2));

			if (cross(center - p3, p4 - p3) > 0)
			{
				momentOfInertia += momentOfInertiaPart1;
			}
			else
			{
				momentOfInertia -= momentOfInertiaPart1;
			}

			if (cross(p4 - p3, p2 - p3) > 0)
			{
				momentOfInertia += momentOfInertiaPart2;
			}
			else
			{
				momentOfInertia -= momentOfInertiaPart2;
			}
		}

		return std::abs(momentOfInertia);
	}

	bool isPointInsideConvexPolygon(const glm::vec2& point, const std::vector<glm::vec2>& points)
	{
		for (size_t i = 1; i < points.size() - 1; ++i)
		{
			const glm::vec2& p1 = points[0];
			const glm::vec2& p2 = points[i];
			const glm::vec2& p3 = points[i + 1 % points.size()];

			if (isPointInsideTriangle(point, p1, p2, p3))
			{
				return true;
			}
		}
		return false;
	}

	bool isPointInsideTriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
		auto coords = calcBarycentricCoords(p, a, b, c);
		return coords.x >= 0 && coords.y >= 0 && coords.z >= 0;
	}
}
