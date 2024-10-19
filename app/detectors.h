#pragma once

#include "shape.h"
#include "box.h"
#include "circle.h"

bool detectCollision(const Shape& sh1, const Shape& sh2);

bool detectCollisionBB(const Box& b1, const Box& b2);
bool detectCollisionCC(const Circle& c1, const Circle& c2);
bool detectCollisionBC(const Box& b, const Circle& c);