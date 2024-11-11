#pragma once

#include "glm/glm.hpp"
#include "collision.h"
#include "shape.h"

Collision resolveCollision(const Shape& sh1, const Shape& sh2);