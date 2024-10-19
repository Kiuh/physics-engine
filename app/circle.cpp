#include "circle.h"
#include "circle_circle_resolver.h"

Circle::Circle(Transform& tr) : Shape(tr)
{
	radius = 1;
	color = Color::randomColor();
}

std::vector<Vertex> Circle::getVertexes() const
{
	std::vector<Vertex> vertices{};

	auto shift = glm::vec2(-radius, 0);
	for (size_t i = 0; i < getVertexesCount(); i++)
	{
		Vertex vert(tr.pos() + shift, color.getValue());
		vertices.push_back(vert);
		rotateVec2(shift, one_seg_deg);
	}
	return vertices;
}

size_t Circle::getVertexesCount() const
{
	return static_cast<size_t>(circle_tris + 1);
}

std::vector<size_t> Circle::getIndexes() const
{
	std::vector<size_t> result{};
	size_t ind = 1;
	for (size_t i = 0; i < circle_tris; i++)
	{
		result.push_back(0);
		result.push_back(ind);
		result.push_back(ind + 1);
		ind += 1;
	}
	result[result.size() - 1] = 1;
	return result;
}

size_t Circle::getIndexesCount() const
{
	return static_cast<size_t>(3 * circle_tris);
}

Collision Circle::getCollision(Shape& other) const
{
	if (auto circ = dynamic_cast<Circle*>(&other); circ != nullptr)
		return _getCollision(*this, *circ);

	return {};
}

bool Circle::isCollide(Shape& other) const
{
	if (auto circ = dynamic_cast<Circle*>(&other); circ != nullptr)
		return _isCollide(*this, *circ);

	return {};
}