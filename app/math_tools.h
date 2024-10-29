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
	bool is_eq_f(float f1, float f2);
	bool is0_f(float f);
	float clamp_m11(float f);
	bool isParallel(glm::vec2 vec1, glm::vec2 vec2);
	glm::vec2 rotate90(glm::vec2 vec);
	bool in_range(float val, float a, float b);
	bool in_range(glm::vec2 val, glm::vec2 a, glm::vec2 b);
	bool greater(glm::vec2 source, glm::vec2 comparison);
	bool lesser(glm::vec2 source, glm::vec2 comparison);

	struct Section
	{
		glm::vec2 p1;
		glm::vec2 p2;

		Section(glm::vec2 p1, glm::vec2 p2);
		bool isSectionPoint(const glm::vec2 dot) const;
	};

	struct Line
	{
		glm::vec2 p;
		glm::vec2 n;

		Line(glm::vec2 p1, glm::vec2 p2);
		Line(Section s);

		std::optional<float> getX(float y) const;
		std::optional<float> getY(float x) const;

		glm::vec2 getNormDirVec() const;
	};

	struct Circle
	{
		glm::vec2 p;
		float r;

		Circle(glm::vec2 p, float r);

		bool isInside(const glm::vec2 dot) const;
	};

	float distance(Line l, glm::vec2 p);
	float getSegmentOverlap(float a1, float b1, float a2, float b2);

	std::optional<glm::vec2> intersection(const Line l1, const Line l2);
	std::vector<glm::vec2> intersection(const Circle c, const Line l);
	std::vector<glm::vec2> intersection(const Circle c, const Section s);
	std::optional<glm::vec2> intersection(const Section s1, const Section s2);
}