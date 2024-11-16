#include "detectors.h"

bool detectCollision(Shape& sh1, Shape& sh2)
{
	auto gjk = new GJK2D();
	return gjk->test(&sh1, &sh2);
}