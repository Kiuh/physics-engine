#include "color.h"

Color::Color()
{
	this->value = glm::vec3(1, 1, 1);
}

Color::Color(glm::vec3 value)
{
	this->value = value;
}

glm::vec3 Color::getValue() const
{
	return value;
}

Color Color::red() { return Color({ 1, 0, 0 }); }
Color Color::white() { return Color({ 1, 1, 1 }); }

Color Color::randomColor()
{
	auto values = glm::vec3{
		randomFloat01(),
		randomFloat01(),
		randomFloat01(),
	};
	return Color(values);
}
