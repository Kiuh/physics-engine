#pragma once

#include "data_manager.h"
#include "physics_engine.h"
#include "shape_creator.h"
#include "object.h"
#include <limits>

class Controller
{
	private:
	WindowManager* win;
	DataManager* data;
	PhysicsEngine* engine;

	std::vector<Object*> objects{};
	std::vector<Shape*> gizmo{};
	size_t initialBoxCount = 2;
	size_t initialCircleCount = 2;
	size_t initialPolygonsCount = 2;

	public:
	Controller(WindowManager* win, DataManager* data, PhysicsEngine* engine);
	~Controller();

	private:
	void setup();
	void createGround();
	const glm::vec2 getRandomPos();
	void createBoxes();
	void createCircles();
	void createPolygons();
	void fillRepresentations();
	void cleanup();
	void processKeyPress(KeyCode key);
};
