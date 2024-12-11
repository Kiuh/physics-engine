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
	size_t initialBoxCount = 1;
	size_t initialCircleCount = 1;
	size_t initialPolygonsCount = 1;

	float restitution = 0.6f;
	float density = 2.0f;
	bool snapToPointer = false;
	int snapIndex = 1;

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
	void mouseMoved(glm::vec2 pos);
	void mouseButton(MouseButton mouseButton);
	void addPolygon();
	void debugUI();
};
