#include "detectors.h"

static std::vector<glm::vec2> getMinkovskyDifference(
	std::vector<glm::vec2> first,
	std::vector<glm::vec2> second
)
{
	std::vector<glm::vec2> result{};

	for (auto& f : first)
	{
		for (auto& s : second)
		{
			result.push_back(f - s);
		}
	}

	return result;
}

static bool isZeroInTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	float cross1 = (p2.x - p1.x) * (-p1.y) - (p2.y - p1.y) * (-p1.x);
	float cross2 = (p3.x - p2.x) * (-p2.y) - (p3.y - p2.y) * (-p2.x);
	float cross3 = (p1.x - p3.x) * (-p3.y) - (p1.y - p3.y) * (-p3.x);

	bool hasSameSign = (cross1 >= 0 && cross2 >= 0 && cross3 >= 0)
		|| (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);

	return hasSameSign;
}

// GJK Collision Detection
bool detectCollision(const Shape& sh1, const Shape& sh2)
{
	auto sh1_points = sh1.getWorldPoints();
	auto sh2_points = sh2.getWorldPoints();

	auto mink_diff = getMinkovskyDifference(sh1_points, sh2_points);

	for (size_t i = 1; i < mink_diff.size() - 1; i++)
	{
		if (isZeroInTriangle(mink_diff[0], mink_diff[i], mink_diff[i + 1]))
		{
			return true;
		}
	}

	return false;
}