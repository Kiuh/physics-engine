#include "box.h"

Box::Box(Transform& tr) : Shape(tr)
{
	halfSize = glm::vec2{ 1.0f,1.0f };
	color = randomColor();
}

void Box::setHalfSize(glm::vec2 halfSize)
{
	this->halfSize = halfSize;
}

glm::vec2 Box::max() const
{
	return tr.pos() + halfSize;
}

glm::vec2 Box::min() const
{
	return tr.pos() - halfSize;
}

glm::vec2 Box::getPos(Corner corner) const
{
	switch (corner)
	{
		case LeftUp: return { min().x, max().y };
		case RightUp: return max();
		case LeftDown: return min();
		case RightDown: return { max().x, min().y };
		default: throw std::runtime_error("Invalid input corner value");
	}
}

float Box::getDegrees(Corner corner) const
{
	auto vec = getPos(corner) - tr.pos();
	auto angle = glm::orientedAngle(glm::vec2(1.0f, 0), glm::normalize(vec));
	return glm::degrees(angle);
}

glm::vec2 Box::size() const
{
	return halfSize * 2.0f;
}

std::vector<glm::vec2> Box::getPoints() const
{
	return { getPos(LeftDown), getPos(LeftUp), getPos(RightUp), getPos(RightDown) };
}

std::vector<Vertex> Box::getVertexes() const
{
	std::vector<Vertex> vertices(4);
	for (size_t i = 0; i < 4; i++)
	{
		vertices[i].color = color.getValue();
	}
	vertices[0].pos = min();
	vertices[1].pos = glm::vec2(min().x, max().y);
	vertices[2].pos = max();
	vertices[3].pos = glm::vec2(max().x, min().y);
	return vertices;
}

size_t Box::getVertexesCount() const
{
	return 4;
}

std::vector<size_t> Box::getIndexes() const
{
	return { 0, 1, 2, 2, 3, 0 };
}

size_t Box::getIndexesCount() const
{
	return 6;
}