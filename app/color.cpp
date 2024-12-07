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

ImU32 Color::getImGuiColor() const
{
	int r = static_cast<int>(glm::clamp(value.r, 0.0f, 1.0f) * 255.0f);
	int g = static_cast<int>(glm::clamp(value.g, 0.0f, 1.0f) * 255.0f);
	int b = static_cast<int>(glm::clamp(value.b, 0.0f, 1.0f) * 255.0f);
	return IM_COL32(r, g, b, 255);
}

Color Color::red() { return Color({ 1, 0, 0 }); }
Color Color::blue() { return Color({ 0, 0, 1 }); }
Color Color::white() { return Color({ 1, 1, 1 }); }
Color Color::green() { return Color({ 0, 128.0f / 256.0f,0 }); }

Color Color::randomColor()
{
	auto values = glm::vec3{
		randomFloat01(),
		randomFloat01(),
		randomFloat01(),
	};
	return Color(values);
}
