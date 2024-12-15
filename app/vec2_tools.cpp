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
		if (points.size() < 3)
		{
			throw std::invalid_argument("Polygon must have at least 3 points.");
		}

		float totalArea = 0.0f;
		float totalMoment = 0.0f;

		size_t numPoints = points.size();

		for (size_t i = 0; i < numPoints; ++i)
		{
			const glm::vec2& p1 = points[i];
			const glm::vec2& p2 = points[(i + 1) % numPoints];

			// Calculate the signed area contribution of the current triangle (center, p1, p2)
			float triangleArea = 0.5f * ((p1.x - center.x) * (p2.y - center.y) - (p2.x - center.x) * (p1.y - center.y));

			// Add to total area (absolute value for total area)
			totalArea += triangleArea;

			// Calculate moment of inertia contribution for the triangle
			float Ix = (p1.y * p1.y + p1.y * p2.y + p2.y * p2.y);
			float Iy = (p1.x * p1.x + p1.x * p2.x + p2.x * p2.x);
			totalMoment += std::abs(triangleArea) * (Ix + Iy) / 12.0f;
		}

		return totalMoment * density;
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
