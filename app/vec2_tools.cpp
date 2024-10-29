#include "vec2_tools.h"

void rotateVec2(glm::vec2& v, float angle)
{
	float rad = glm::radians(angle);
	glm::mat2 rotationMatrix = glm::mat2(
		glm::cos(rad), -glm::sin(rad),
		glm::sin(rad), glm::cos(rad)
	);
	v = rotationMatrix * v;
}