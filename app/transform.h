#pragma once

#include "glm/glm.hpp"

struct Transform
{
	private:
	float rotation;
	glm::vec2 position;

	public:
	Transform();

	glm::vec2 pos() const;
	void setPos(glm::vec2 pos);
	void movePos(glm::vec2 shift);

	float rot() const;
	void setRot(float rot);
	void rotate(float shift);

	glm::vec2 localToWorld(glm::vec2 point) const;
	glm::vec2 worldToLocal(glm::vec2 point) const;
};