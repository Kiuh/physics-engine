#pragma once

#include "color.h"
#include "data_manager.h"
#include "vec2_tools.h"
#include <glm/glm.hpp>
#include <imgui.h>
#include <vector>

void drawDots(const DataManager& dm, const std::vector<glm::vec2>& dots);
void drawConvexHulls(const DataManager& dm, const std::vector<std::vector<glm::vec2>>& hulls, const Color& color = Color::blue());
void drawLineSegments(const DataManager& dm, const std::vector<std::vector<glm::vec2>>& lines, const Color& color = Color::blue());