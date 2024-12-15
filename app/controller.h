#pragma once

#include "collision_2d.h"
#include "data_manager.h"
#include "object.h"
#include "physics_engine.h"
#include "shape.h"
#include "shape_creator.h"
#include "vec2_tools.h"
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
	size_t initialBoxCount;
	size_t initialCircleCount;
	size_t initialPolygonsCount;

	bool snapToPointer;
	int snapIndex;
	bool showRigidBodyTooltip;
	bool showRigidBodyCenterOfMass;

	public:
	Controller(Debug* debug, WindowManager* win, DataManager* data, PhysicsEngine* engine);
	~Controller();

	private:
	void setup();

	bool isNoIntersections(Object& obj);
	const glm::vec2 getRandomPos();
	void createAndPlace(Shape& shape);

	void createGround();
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
