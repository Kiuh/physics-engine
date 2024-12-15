#include "imgui_helper.h"

void drawPlus(const DataManager& dm, const glm::vec2& _center, float size, ImU32 color, float thickness)
{
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	glm::vec2 center = dm.worldToScreenCoord(_center);

	ImVec2 horizontalStart = ImVec2(center.x - size / 2, center.y);
	ImVec2 horizontalEnd = ImVec2(center.x + size / 2, center.y);
	ImVec2 verticalStart = ImVec2(center.x, center.y - size / 2);
	ImVec2 verticalEnd = ImVec2(center.x, center.y + size / 2);

	draw_list->AddLine(horizontalStart, horizontalEnd, color, thickness);
	draw_list->AddLine(verticalStart, verticalEnd, color, thickness);
}

void drawDots(const DataManager& dm, const std::vector<glm::vec2>& dots)
{
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	for (auto& dot : dots)
	{
		glm::vec2 pos = dm.worldToScreenCoord(dot);
		drawList->AddCircleFilled({ pos.x, pos.y }, 4.0f, IM_COL32(0, 255, 0, 255));
	}
}

void drawConvexHulls(const DataManager& dm, const std::vector<std::vector<glm::vec2>>& lines, const Color& color, const float thickness)
{
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	for (auto& line : lines)
	{
		auto line_dots = vt::createConvexHull(line);
		std::vector<ImVec2> dots{};
		for (auto& dot : line_dots)
		{
			glm::vec2 pos = dm.worldToScreenCoord(dot);
			dots.push_back({ pos.x, pos.y });
		}
		drawList->AddPolyline(
			dots.data(),
			static_cast<int>(dots.size()),
			color.getImGuiColor(),
			ImDrawFlags_Closed,
			thickness
		);
	}
}

void drawLineSegments(const DataManager& dm, const std::vector<std::vector<glm::vec2>>& lines, const Color& color)
{
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	for (auto& line : lines)
	{
		std::vector<ImVec2> dots{};
		for (auto& dot : line)
		{
			glm::vec2 pos = dm.worldToScreenCoord(dot);
			dots.push_back({ pos.x, pos.y });
		}
		drawList->AddPolyline(
			dots.data(),
			static_cast<int>(dots.size()),
			color.getImGuiColor(),
			ImDrawFlags_None,
			2.0f
		);
	}
}
