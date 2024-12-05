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
	rotation = rot;
}

void Transform::rotate(float shift)
{
	rotation += shift;
	rotation = std::fmodf(rotation, 360.0f);
}

glm::vec2 Transform::localToWorld(glm::vec2 point) const
{
	return point + position;
}

glm::vec2 Transform::worldToLocal(glm::vec2 point) const
{
	return point - position;
}
