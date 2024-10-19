#pragma once

#include "box.h"
#include "circle.h"

struct BoxCircleTouchParams
{
	float boxSide;
	float distance;
};

BoxCircleTouchParams getParams(const Box& box, const Circle& circle);
bool isTouchBoxCircle(const Box& box, const Circle& circle);
Collision getBoxCircleCollision(const Box& box, const Circle& circle);