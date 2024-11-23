#pragma once

#include "GJK2D.h"
#include "collision.h"
#include "shape.h"
#include <memory>

bool isOverlaps(Shape& sh1, Shape& sh2);
std::optional<Collision> getCollision(Shape& sh1, Shape& sh2);
std::optional<std::vector<glm::vec2>> getWorldContactPoints(Shape& sh1, Shape& sh2);