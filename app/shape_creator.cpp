#include "shape_creator.h"

Shape& createBoxShape(Transform& tr, const glm::vec2& extends)
{
	auto shape = new Shape(tr);

	std::vector<glm::vec2> points{};
	points.push_back(extends * glm::vec2{ -1, -1 });
	points.push_back(extends * glm::vec2{ -1, 1 });
	points.push_back(extends * glm::vec2{ 1, 1 });
	points.push_back(extends * glm::vec2{ 1, -1 });
	shape->setLocalPoints(points);

	return *shape;
}

static constexpr size_t circle_segment_splits = 4;
static constexpr size_t circle_points = 4 * (1 + circle_segment_splits);
static constexpr float one_seg_deg = 360.0f / circle_points;

Shape& createCircleShape(Transform& tr, const float& radius)
{
	auto shape = new Shape(tr);

	std::vector<glm::vec2> points{};
	auto shift = glm::vec2(-radius, 0);
	for (size_t i = 0; i < circle_points; i++)
	{
		points.push_back(shift);
		shift = vt::rotateVec2(shift, one_seg_deg);
	}
	shape->setLocalPoints(points);

	return *shape;
}

Shape& createPolygonShape(Transform& tr, std::vector<glm::vec2> points)
{
	auto shape = new Shape(tr);
	shape->setLocalPoints(points);
	return *shape;
}

Shape& createRandomPolygonShape(Transform& tr, const long& count, const float& radius)
{
	std::vector<glm::vec2> points{};
	auto shift = glm::vec2(-radius, 0);
	for (size_t i = 0; i < count; i++)
	{
		points.push_back(shift);
		shift = vt::rotateVec2(shift, 360.0f / count);
	}
	return createPolygonShape(tr, points);
}
