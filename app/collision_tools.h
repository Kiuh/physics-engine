#pragma once

#include "GJK2D.h"
#include "collision.h"
#include "shape.h"

bool isOverlaps(Shape& sh1, Shape& sh2);
std::optional<Collision> intersection(Shape& sh1, Shape& sh2);