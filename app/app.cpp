#pragma once

#include "data_manager.cpp"
#include "fps_counter.cpp"
#include "graphic_engine.cpp"
#include "physics_engine.cpp"
#include "window_manager.cpp"
#include "controller.cpp"

#include <cstdint>
#include <exception>
#include "glm/glm.hpp"
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
	App()
	{
		graphicsEngineConfig = GraphicsEngineConfig{};
#ifdef NDEBUG
		graphicsEngineConfig.isDebug = false;
#else
		graphicsEngineConfig.isDebug = true;
#endif
		graphicsEngineConfig.maxFramesInFlight = 2;

		this->windowManager = std::make_unique<WindowManager>(size, title);
		this->dataManager = std::make_unique<DataManager>(windowManager.get());

		this->physicsEngine = std::make_unique<PhysicsEngine>();
		this->graphicsEngine = std::make_unique<GraphicEngine>(windowManager.get(), dataManager.get(), graphicsEngineConfig);

		this->controller = std::make_unique<Controller>(windowManager.get(), dataManager.get(), physicsEngine.get());

		this->graphicFrameCounter = std::make_unique<FpsCounter>("Graphic FPS");
		this->physicFrameCounter = std::make_unique<FpsCounter>("Physics FPS");
	}

	void run()
	{
		isRunning = true;

		graphicFrameCounter->run();
		physicFrameCounter->run();

		graphicThread = std::make_unique<std::thread>(&App::graphicThreadFunc, this);
		physicsThread = std::make_unique<std::thread>(&App::physicsThreadFunc, this);

		while (!windowManager->isShouldClose())
		{
			windowManager->poolEvents();
		}
		isRunning = false;

		physicsThread->join();
		graphicThread->join();
	}

	private:
	void graphicThreadFunc()
	{
		while (isRunning)
		{
			graphicsEngine->draw();
			graphicFrameCounter->tick();
		}
	}

	void physicsThreadFunc()
	{
		const auto target_delta_time = duration_cast<std::chrono::microseconds>(std::chrono::seconds{ 1 }) / targetPhysicsFps;
		auto deltaTime = std::chrono::duration<float>{ 0.0f };

		while (isRunning)
		{
			auto timer = std::chrono::high_resolution_clock::now();
			physicsEngine->update(deltaTime.count());
			physicFrameCounter->tick();

			auto time_passed = duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer);
			auto time_left = target_delta_time - time_passed;

			if (time_left >= std::chrono::microseconds{ 0 })
			{
				std::this_thread::sleep_for(time_left);
			}

			deltaTime = (std::chrono::high_resolution_clock::now() - timer);
		}
	}
};