#pragma once

#include <optional>
#include "glm/glm.hpp"
#include <glm/gtc/epsilon.hpp>
#include <glm/ext/scalar_constants.hpp>

static inline bool is_eq_f(float f1, float f2)
{
	return glm::epsilonEqual(f1, f2, glm::epsilon<float>());
}

static inline bool is0_f(float f)
{
	return is_eq_f(f, 0.0f);
}

struct Line
{
	glm::vec2 p;
	glm::vec2 n;

	glm::vec3 v3Normal()
	{
		return { n.x, n.y, 0 };
	}

	Line(glm::vec2 p1, glm::vec2 p2)
	{
		p = p1;
		n = p2 - p1;

		auto x = n.x;
		n.x = -n.y;
		n.y = x;
	}

	std::optional<float> getX(float y) const
	{
		if (is0_f(n.x)) return {};
		return (-n.y * (y - p.y) / n.x) + p.x;
	}

	std::optional<float> getY(float x) const
	{
		if (is0_f(n.y)) return {};
		return (-n.x * (x - p.x) / n.y) + p.y;
	}
};

static std::optional<glm::vec2> intersection(Line l1, Line l2)
{
	auto cross = glm::cross(l1.v3Normal(), l2.v3Normal());
	auto len = glm::length(cross);
	if (is0_f(len))
	{
		return {};
	}

	// Formula from:
	// (general) nx * (x - x0) + ny * (y - y0) = 0
	// and solve system from each line
	auto top = l1.n.x * l2.n.x * (l2.p.x - l1.p.x) + l1.n.x * l2.n.y * l2.p.y - l2.n.x * l1.n.y * l1.p.y;
	auto bottom = l1.n.x * l2.n.y - l2.n.x * l1.n.y;
	auto y = top / bottom;
	return {
		glm::vec2{
			l1.getX(y).value(),
			y
		},
	};
}