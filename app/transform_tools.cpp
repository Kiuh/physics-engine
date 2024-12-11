#include "transform_tools.h"

glm::vec2 localToWorld(const glm::vec2& point, const Transform& tr)
{
	auto result = vt::rotateVec2(point, tr.rot());
	return result + tr.pos();
}

glm::vec2 worldToLocal(const glm::vec2& point, const Transform& tr)
{
	auto result = point - tr.pos();
	return vt::rotateVec2(result, -tr.rot());
}
