#pragma once

#include "color.h"
#include "data_manager.h"
#include "vec2_tools.h"
#include <glm/glm.hpp>
#include <imgui.h>
#include <vector>

void drawPlus(const DataManager& dm, const glm::vec2& center, float size, ImU32 color, float thickness);
void drawDots(const DataManager& dm, const std::vector<glm::vec2>& dots);
void drawConvexHulls(
	const DataManager& dm,
	const std::vector<std::vector<glm::vec2>>& hulls,
	const Color& color = Color::blue(),
	const float thikness = 2.0f
);
void drawLineSegments(
	const DataManager& dm,
	const std::vector<std::vector<glm::vec2>>& lines,
	const Color& color = Color::blue()
);