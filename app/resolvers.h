#pragma once

#include "glm/glm.hpp"
#include "math_tools.h"
#include "box.h"
#include "circle.h"
#include "collision.h"
#include "box_circle_tools.h"

#include <glm/gtx/vector_angle.hpp>
#include <iostream>

Collision createCollision(const Shape& sh1, const Shape& sh2);

Collision createCollisionBB(const Box& b1, const Box& b2);
Collision createCollisionCC(const Circle& c1, const Circle& c2);
Collision createCollisionBC(const Box& b, const Circle& c);
Collision createCollisionCB(const Circle& c, const Box& b);