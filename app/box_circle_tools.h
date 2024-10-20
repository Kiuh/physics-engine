#pragma once

#include "box.h"
#include "circle.h"

std::vector<glm::vec2> getIntersectionsPoints(const Box& box, const Circle& circle);