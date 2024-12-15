#include "transform.h"

Transform::Transform()
{
	rotation = 0;
	position = { 0,0 };
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
	position += shift;
}

float Transform::rot() const
{
	return rotation;
}

void Transform::setRot(float rot)
{
	rotation = std::fmodf(rot, 360.0f);
}

void Transform::rotate(float shift)
{
	rotation += glm::degrees(shift);
	rotation = std::fmodf(rotation, 360.0f);
}
