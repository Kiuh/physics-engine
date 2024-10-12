#pragma once

#include "glm/glm.hpp"

struct Transform
{
	private:
	float orientation;
	glm::vec2 position;
	glm::vec2 rotation;

	public:
	Transform()
	{
		orientation = 0;
		position = { 0,0 };
		rotation = { 0,0 };
	}

	glm::vec2 getPos() const
	{
		return position;
	}

	void setPosition(glm::vec2 position)
	{
		this->position = position;
	}

	void movePosition(glm::vec2 shift)
	{
		this->position += shift;
	}
};