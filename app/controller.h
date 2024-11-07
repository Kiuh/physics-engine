#pragma once

#include "data_manager.h"
#include "physics_engine.h"
#include "object.h"
#include <limits>

class Controller
{
	private:
	WindowManager* win;
	DataManager* data;
	PhysicsEngine* engine;

	std::vector<Object*> objects{};
	size_t initialBoxCount = 5;
	size_t initialCircleCount = 5;

	public:
	Controller(WindowManager* win, DataManager* data, PhysicsEngine* engine);
	~Controller();

	private:
	void setup();
	void createGround();
	const glm::vec2 getRandomPos();
	void createBoxes();
	void createCircles();
	bool isNoIntersections(Object& obj);
	void fillRepresentations();
	void cleanup();
	void processKeyPress(KeyCode key);
};