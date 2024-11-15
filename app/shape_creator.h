#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "shape.h"
#include "color.h"
#include "vec2_tools.h"

Shape& createBoxShape(Transform& tr, const glm::vec2& extends);
Shape& createCircleShape(Transform& tr, const float& radius);
Shape& createPolygonShape(Transform& tr, std::vector<glm::vec2> points);
Shape& createRandomPolygonShape(Transform& tr, const long& count, const float& radius);