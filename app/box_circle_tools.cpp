#include "box_circle_tools.h"

std::vector<SectionWithPoints> getIntersectionsPoints(const Box& box, const Circle& circle)
{
	auto c = mt::Circle(circle.tr.pos(), circle.radius);

	auto points = box.getPoints();
	std::vector<SectionWithPoints> inters{};
	for (size_t i = 0; i < points.size(); i++)
	{
		auto sec = mt::Section(points[i], points[(i + 1) % points.size()]);
		auto vec = mt::intersection(c, sec);
		if (vec.size() != 0)
		{
			inters.push_back({ sec,vec });
		}
	}

	return inters;
}