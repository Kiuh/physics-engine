#pragma once

#include "vertex_source.h"
#include "vertex.h"
#include "window_manager.h"
#include <cstdint>
#include <mutex>
#include <vector>
#include "vulkan_vec.h"
#include "shape.h"

constexpr float MIN_PIXELS_PER_UNIT = 10;
constexpr float MAX_PIXELS_PER_UNIT = 400;
constexpr float MOUSE_SENSETIVE = 0.04f;

class DataManager
{
	private:
	WindowManager& window;
	float pixelsPerUnit = 50;
	glm::vec2 zeroShift{};

	public:
	std::vector<VertexSource*> dataSources{};
	vulkan_vec<Vertex> simplex_points{ };
	vulkan_vec<uint16_t> indexes{ };
	std::mutex data_mutex{};
	boost::signals2::signal<void()> dataStructureChanged{};

	DataManager(WindowManager& window);
	void setPixelPerUnit(float ppu);
	void handleMouseScroll(float value);
	void handlePressedMouseMove(glm::vec2 delta);
	void notifyStructureChanging();
	void createDataSpace();
	void recalculateVertexes();

	private:
	void worldToScreen(Vertex& vert, WindowManager& wp, float pixelsPerUnit) const;
};