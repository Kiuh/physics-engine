#include "transform.h"

Transform::Transform()
{
	orientation = 0;
	position = { 0,0 };
	rotation = { 0,0 };
}

glm::vec2 Transform::pos() const
{
	return position;
}

void Transform::setPos(glm::vec2 position)
{
	this->position = position;
}

void Transform::movePos(glm::vec2 shift)
{
	this->position += shift;
}
