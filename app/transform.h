#pragma once

#include "glm/glm.hpp"

struct Transform
{
	private:
	float orientation;
	glm::vec2 position;
	glm::vec2 rotation;

	public:
	Transform();
	glm::vec2 pos() const;
	void setPos(glm::vec2 position);
	void movePos(glm::vec2 shift);
};