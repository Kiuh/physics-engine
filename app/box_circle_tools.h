#pragma once

#include "box.h"
#include "circle.h"

struct SectionWithPoints
{
	mt::Section section;
	std::vector<glm::vec2> points;
};

std::vector<SectionWithPoints> getIntersectionsPoints(const Box& box, const Circle& circle);