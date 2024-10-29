#include "math_tools.h"

namespace mt
{
	bool is_eq_f(float f1, float f2)
	{
		return glm::epsilonEqual(f1, f2, glm::epsilon<float>());
	}

	bool is0_f(float f)
	{
		return is_eq_f(f, 0.0f);
	}

	float clamp_m11(float f)
	{
		return glm::clamp(f, -1.0f, 1.0f);
	}

	bool isParallel(glm::vec2 vec1, glm::vec2 vec2)
	{
		auto angle = glm::degrees(glm::angle(vec1, vec2));
		return is0_f(angle) || is0_f(angle - 180.0f);
	}

	glm::vec2 rotate90(glm::vec2 vec)
	{
		auto x = vec.x;
		vec.x = -vec.y;
		vec.y = x;
		return vec;
	}

	bool in_range(float val, float a, float b)
	{
		return val >= fmin(a, b) - glm::epsilon<float>() && val <= fmax(a, b) + glm::epsilon<float>();
	}

	bool in_range(glm::vec2 val, glm::vec2 a, glm::vec2 b)
	{
		return in_range(val.x, a.x, b.x) && in_range(val.y, a.y, b.y);
	}

	bool greater(glm::vec2 source, glm::vec2 comparison)
	{
		return source.x >= comparison.x && source.y >= comparison.y;
	}

	bool lesser(glm::vec2 source, glm::vec2 comparison)
	{
		return source.x <= comparison.x && source.y <= comparison.y;
	}


	Section::Section(glm::vec2 p1, glm::vec2 p2) : p1{ p1 }, p2{ p2 }
	{
	}

	bool Section::isSectionPoint(const glm::vec2 dot) const
	{
		return
			isParallel(p2 - p1, p2 - dot)
			&& in_range(dot, p1, p2);
	}

	Line::Line(glm::vec2 p1, glm::vec2 p2)
	{
		p = p1;
		n = p2 - p1;
		n = rotate90(n);
	}

	Line::Line(Section s) : Line(s.p1, s.p2)
	{
	}

	std::optional<float> Line::getX(float y) const
	{
		if (is0_f(n.x)) return {};
		return (-n.y * (y - p.y) / n.x) + p.x;
	}

	std::optional<float> Line::getY(float x) const
	{
		if (is0_f(n.y)) return {};
		return (-n.x * (x - p.x) / n.y) + p.y;
	}

	glm::vec2 Line::getNormDirVec() const
	{
		return glm::normalize(rotate90(n));
	}

	Circle::Circle(glm::vec2 p, float r) : p{ p }, r{ r }
	{
	}

	bool Circle::isInside(const glm::vec2 dot) const
	{
		auto vec = dot - p;
		return abs(vec.x * vec.x + vec.y * vec.y) <= r * r + glm::epsilon<float>();
	}

	std::optional<glm::vec2> intersection(const Line l1, const Line l2)
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

	float distance(Line l, glm::vec2 p)
	{
		auto l_n = Line(p, p + rotate90(l.n));
		auto p2 = intersection(l, l_n);
		return glm::distance(p, p2.value());
	}

	std::vector<glm::vec2> intersection(const Circle c, const Line l)
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
		float shift = glm::sqrt(c.r * c.r - d * d);
		auto sh_dir = rotate90(l.n);

		auto ans1 = p2 + sh_dir * shift;
		auto ans2 = p2 + -sh_dir * shift;

		return  { ans1, ans2 };
	}

	std::vector<glm::vec2> intersection(const Circle c, const Section s)
	{
		std::vector<glm::vec2> result{};
		auto ans = intersection(c, Line(s));
		for (auto& v : ans)
		{
			if (s.isSectionPoint(v))
			{
				result.push_back(v);
			}
		}
		return result;
	}

	std::optional<glm::vec2> intersection(const Section s1, const Section s2)
	{
		auto inters = intersection(Line(s1), Line(s2));
		if (inters.has_value() && !s1.isSectionPoint(inters.value())) return {};
		return inters;
	}

	float getSegmentOverlap(float a1, float b1, float a2, float b2)
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