#pragma once

#include "controller.h"
#include "data_manager.h"
#include "fps_counter.h"
#include "graphic_engine.h"
#include "physics_engine.h"
#include "window_manager.h"

#include "glm/glm.hpp"
#include <cstdint>
#include <exception>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <thread>   

class App
{
	private:
	glm::ivec2 size{ 2000, 1000 };
	std::string title = "Physics Simulation";
	uint32_t targetPhysicsFps = 120;
	bool isRunning = false;

	GraphicsEngineConfig graphicsEngineConfig;

	std::unique_ptr<WindowManager> windowManager;
	std::unique_ptr<DataManager> dataManager;

	std::unique_ptr<GraphicEngine> graphicsEngine;
	std::unique_ptr<PhysicsEngine> physicsEngine;

	std::unique_ptr<Controller> controller;

	std::unique_ptr<FpsCounter> graphicFrameCounter;
	std::unique_ptr<FpsCounter> physicFrameCounter;

	std::unique_ptr<std::thread> graphicThread;
	std::unique_ptr<std::thread> physicsThread;

	public:
	App();
	void run();

	private:
	void processKeyPress(KeyCode key);
	void graphicThreadFunc();
	void physicsThreadFunc();
};