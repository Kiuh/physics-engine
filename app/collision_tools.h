#pragma once

#include "shape.h"
#include "collision.h"
#include "GJK2D.h"

bool isOverlaps(Shape& sh1, Shape& sh2);
std::optional<Collision> intersection(Shape& sh1, Shape& sh2);