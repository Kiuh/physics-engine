#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/glm.hpp"
#include "math_tools.hpp"
#include "box.h"
#include "circle.h"
#include "collision.hpp"
#include <glm/gtx/vector_angle.hpp>
#include <iostream>

bool _isCollide(const Box& b, const Circle& c);
Collision _getCollision(const Box& b, const Circle& c);
Collision _getCollision(const Circle& c, const Box& b);