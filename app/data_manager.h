#pragma once

#include "shape.h"
#include "vertex.h"
#include "vertex_source.h"
#include "vulkan_vec.h"
#include "window_manager.h"
#include <cstdint>
#include <mutex>
#include <vector>

constexpr float MIN_PIXELS_PER_UNIT = 10;
constexpr float MAX_PIXELS_PER_UNIT = 400;
constexpr float MOUSE_SENSETIVE = 0.04f;

class DataManager
{
	private:
	WindowManager& window;

	public:
	float pixelsPerUnit = 50;
	glm::vec2 zeroShift{};

	std::vector<VertexSource*> dataSources{};
	vulkan_vec<Vertex> simplex_points{ };
	vulkan_vec<uint16_t> indexes{ };
	std::mutex data_mutex{};

	boost::signals2::signal<void()> dataStructureChanged{};

	DataManager(WindowManager& window);
	void setPixelPerUnit(float ppu);
	void handleMouseScroll(float value);
	void handleRightMouseButtonMove(glm::vec2 delta);
	void notifyStructureChanging();
	void createDataSpace();
	void recalculateVertexes();

	glm::vec2 worldToGraphicScreen(const glm::vec2& pos) const;
	glm::vec2 worldToScreenCoord(const glm::vec2& pos) const;
	glm::vec2 screenCoordToWorld(const glm::vec2& pos) const;
};