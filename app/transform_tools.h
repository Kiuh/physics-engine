#pragma once

#include "transform.h"
#include "vec2_tools.h"

#include <glm/glm.hpp>

glm::vec2 localToWorld(const glm::vec2& point, const Transform& tr);
glm::vec2 worldToLocal(const glm::vec2& point, const Transform& tr);