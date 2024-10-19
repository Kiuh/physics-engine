#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <optional>
#include <vector>
#include "glm/glm.hpp"
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/ext/scalar_constants.hpp>

namespace mt
{
	static inline bool is_eq_f(float f1, float f2)
	{
		return glm::epsilonEqual(f1, f2, glm::epsilon<float>());
	}

	static inline bool is0_f(float f)
	{
		return is_eq_f(f, 0.0f);
	}

	static inline glm::vec2 rotate90(glm::vec2 vec)
	{
		auto x = vec.x;
		vec.x = -vec.y;
		vec.y = x;
		return vec;
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
			n = rotate90(n);
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

		glm::vec2 getNormDirVec() const
		{
			return glm::normalize(rotate90(n));
		}
	};

	struct Circle
	{
		glm::vec2 p;
		float r;

		Circle(glm::vec2 p, float r)
		{
			this->p = p;
			this->r = r;
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

	static float distance(Line l, glm::vec2 p)
	{
		auto l_n = Line(p, p + rotate90(l.n));
		auto p2 = intersection(l, l_n);
		return glm::distance(p, p2.value());
	}

	static std::optional<std::vector<glm::vec2>> intersection(Circle c, Line l)
	{
		// Find line normal to current via circle center
		auto l_n = Line(c.p, c.p + rotate90(l.n));

		// Find shortest distance from line to circle
		auto p2_r = intersection(l, l_n);
		auto& p2 = p2_r.value();
		auto d = glm::distance(c.p, p2);

		// Exclusive situation when pass via center
		if (is0_f(d))
		{
			auto vec = l.getNormDirVec() * c.r;
			return { { c.p + vec, c.p - vec } };
		}

		// One intersection
		if (is_eq_f(d, c.r))  return { { p2 } };

		// None intersections
		if (d > c.r) return {};

		// Two intersections => find angle and rotate normal vector
		float alfa = glm::degrees(glm::acos(d / c.r));
		auto t_v = glm::normalize(p2 - c.p) * c.r;

		auto ans1 = c.p + glm::rotate(t_v, alfa);
		auto ans2 = c.p + glm::rotate(t_v, -alfa);

		return { { ans1,ans2 } };
	}
}