#pragma once

#include "collision_2d.h"
#include "data_manager.h"
#include "object.h"
#include "physics_engine.h"
#include "shape.h"
#include "shape_creator.h"
#include "window_manager.h"
#include <glm/fwd.hpp>
#include <vector>

class Controller
{
	private:
	WindowManager* win;
	DataManager* data;
	PhysicsEngine* engine;

	std::vector<Object*> objects{};
	size_t initialBoxCount = 2;
	size_t initialCircleCount = 3;
	size_t initialPolygonsCount = 4;

	float restitution = 0.6f;
	float density = 2.0f;

	public:
	Controller(Debug* debug, WindowManager* win, DataManager* data, PhysicsEngine* engine);
	~Controller();

	private:
	void setup();
	bool isNoIntersections(Object& obj);
	void createGround();
	const glm::vec2 getRandomPos();
	void createBoxes();
	void createCircles();
	void createPolygons();
	void fillRepresentations();
	void cleanup();
	void processKeyPress(KeyCode key);
	void addPolygon();
	void debugUI();
};
