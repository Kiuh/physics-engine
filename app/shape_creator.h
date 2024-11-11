#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "shape.h"
#include "color.h"
#include "vec2_tools.h"

Shape& createBoxShape(Transform& tr, const glm::vec2& extends);
Shape& createCircleShape(Transform& tr, const float& radius);