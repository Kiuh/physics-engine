#include "box_circle_tools.h"

std::vector<glm::vec2> getIntersectionsPoints(const Box& box, const Circle& circle)
{
	auto c = mt::Circle(circle.tr.pos(), circle.radius);

	auto points = box.getPoints();
	std::vector<glm::vec2> inters{};
	for (size_t i = 0; i < points.size(); i++)
	{
		auto vec = mt::intersection(c, mt::Section(points[i], points[(i + 1) % points.size()]));
		for (auto& v : vec)
		{
			inters.push_back(v);
		}
	}

	return inters;
}