#include "transform_tools.h"

glm::vec2 localToWorld(const glm::vec2& point, const Transform& tr)
{
	return point + tr.pos();
}

glm::vec2 worldToLocal(const glm::vec2& point, const Transform& tr)
{
	return point - tr.pos();
}
