#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <stdexcept>
#include <optional>
#include <vector>
#include <cmath>
#include "glm/glm.hpp"
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/vector_angle.hpp>

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

	static inline float clamp_m11(float f)
	{
		return glm::clamp(f, -1.0f, 1.0f);
	}

	static inline bool isParallel(glm::vec2 vec1, glm::vec2 vec2)
	{
		auto angle = glm::degrees(glm::angle(vec1, vec2));
		return is0_f(angle) || is0_f(angle - 180.0f);
	}

	static inline glm::vec2 rotate90(glm::vec2 vec)
	{
		auto x = vec.x;
		vec.x = -vec.y;
		vec.y = x;
		return vec;
	}

	static inline bool greater(glm::vec2 source, glm::vec2 comparison)
	{
		return source.x >= comparison.x && source.y >= comparison.y;
	}

	static inline bool lesser(glm::vec2 source, glm::vec2 comparison)
	{
		return source.x <= comparison.x && source.y <= comparison.y;
	}

	struct Line
	{
		glm::vec2 p;
		glm::vec2 n;

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

		Circle(glm::vec2 p, float r) : p{ p }, r{ r }
		{
		}

		bool isInside(const glm::vec2 dot) const
		{
			auto vec = dot - p;
			return abs(vec.x * vec.x + vec.y * vec.y) <= r * r + glm::epsilon<float>();
		}
	};

	struct Section
	{
		glm::vec2 p1;
		glm::vec2 p2;

		Section(glm::vec2 p1, glm::vec2 p2) : p1{ p1 }, p2{ p2 }
		{
		}

		const glm::vec2 min() const
		{
			return { glm::min(p1.x, p2.x) ,glm::min(p1.y, p2.y) };
		}

		const glm::vec2 max() const
		{
			return { glm::max(p1.x, p2.x) ,glm::max(p1.y, p2.y) };
		}

		bool isSectionPoint(const glm::vec2 dot) const
		{
			return
				isParallel(p2 - p1, p2 - dot)
				&& lesser(dot, max())
				&& greater(dot, min());
		}
	};

	static std::optional<glm::vec2> intersection(Line l1, Line l2)
	{
		if (isParallel(l1.n, l2.n))
		{
			return {};
		}

		// Formula from:
		// (general) nx * (x - x0) + ny * (y - y0) = 0
		// and solve system from each line
		auto l12xy = l1.n.x * l2.n.y;
		auto l21xy = l2.n.x * l1.n.y;
		auto top = l1.n.x * l2.n.x * (l2.p.x - l1.p.x) + l12xy * l2.p.y - l21xy * l1.p.y;
		auto bottom = l12xy - l21xy;
		auto y = top / bottom;

		auto x1 = l1.getX(y);
		auto x2 = l2.getX(y);

		if (!x1.has_value() && !x2.has_value())
		{
			throw std::runtime_error("Impossible situation.");
		}

		return {
			glm::vec2{
				x1.value_or(x2.value_or(0.0f)),
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

	static std::vector<glm::vec2> intersection(Circle c, Line l)
	{
		// Find line normal to current via circle center
		auto l_n = Line(c.p, c.p + l.n);

		// Find shortest distance from line to circle
		auto p2_r = intersection(l, l_n);
		auto& p2 = p2_r.value();
		auto d = glm::distance(c.p, p2);

		// Exclusive situation when pass via center
		if (is0_f(d))
		{
			auto vec = l.getNormDirVec() * c.r;
			return { c.p + vec, c.p - vec };
		}

		// One intersection
		if (is_eq_f(d, c.r))  return { p2 };

		// None intersections
		if (d > c.r) return {};

		// Two intersections => find angle and rotate normal vector
		float alfa = glm::degrees(glm::acos(d / c.r));
		auto t_v = glm::normalize(p2 - c.p) * c.r;

		auto ans1 = c.p + glm::rotate(t_v, alfa);
		auto ans2 = c.p + glm::rotate(t_v, -alfa);

		return  { ans1, ans2 };
	}

	static std::vector<glm::vec2> intersection(const Circle c, const Section s)
	{
		std::vector<glm::vec2> result{};
		auto ans = intersection(c, Line(s.p1, s.p2));
		for (auto& v : ans)
		{
			if (s.isSectionPoint(v))
			{
				result.push_back(v);
			}
		}
		return result;
	}

	static float getSegmentOverlap(float a1, float b1, float a2, float b2)
	{
		float len1 = b1 - a1;
		float len2 = b2 - a2;

		float m1 = a1 + len1 / 2.0f; // middle of 1 seg
		float m2 = a2 + len2 / 2.0f; // middle of 2 seg

		if (a1 >= a2 && b1 <= b2) // fully inside
		{
			if (m1 > m2)
			{
				return len1;
			}
			else
			{
				return -len1;
			}
		}

		if (a1 <= a2 && b1 >= a1 && b1 <= b2) // overlaps only left point
		{
			return -(b1 - a2);
		}

		if (a1 >= a2 && a1 <= b2 && b1 >= b2) // overlaps only right point
		{
			return b2 - a1;
		}

		// fully overlaps
		if (m1 > m2)
		{
			return len2;
		}
		else
		{
			return -len2;
		}
	}
}