#pragma once

#include "data_provider.cpp"
#include "fps_counter.cpp"
#include "graphic_engine.cpp"
#include "physics_engine.cpp"
#include "vertex_transformer.cpp"
#include "window_provider.cpp"

#include <cstdint>
#include <exception>
#include <glm/ext/vector_int2.hpp>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <thread>

using namespace std;
using namespace glm;

class App
{
	private:
	ivec2 size{ 2000, 1000 };
	string title = "Physics Simulation";
	uint32_t targetPhysicsFps = 120;
	bool isRunning = false;

	std::unique_ptr<WindowProvider> windowProvider;
	std::unique_ptr<GraphicEngine> graphicsEngine;
	std::unique_ptr<DataProvider> dataProvider;
	std::unique_ptr<VertexTransformer> vertexTransformer;
	std::unique_ptr<PhysicsEngine> physicsEngine;

	std::unique_ptr<FpsCounter> graphicFrameCounter;
	std::unique_ptr<FpsCounter> physicFrameCounter;

	std::unique_ptr<thread> graphicThread;
	std::unique_ptr<thread> physicsThread;

	public:
	App()
	{
		this->windowProvider = make_unique<WindowProvider>(size, title);
		this->vertexTransformer = make_unique<VertexTransformer>(windowProvider.get());
		this->dataProvider = make_unique<DataProvider>(vertexTransformer.get());
		this->physicsEngine = make_unique<PhysicsEngine>(dataProvider.get());
		this->graphicsEngine = make_unique<GraphicEngine>(windowProvider.get(), dataProvider.get());

		this->graphicFrameCounter = make_unique<FpsCounter>("Graphic FPS");
		this->physicFrameCounter = make_unique<FpsCounter>("Physics FPS");
	}

	void run()
	{
		isRunning = true;

		graphicFrameCounter->run();
		physicFrameCounter->run();

		graphicThread = make_unique<thread>(&App::graphicThreadFunc, this);
		physicsThread = make_unique<thread>(&App::physicsThreadFunc, this);

		while (!windowProvider->isShouldClose())
		{
			windowProvider->poolEvents();
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
		const auto target_delta_time = duration_cast<microseconds>(seconds{ 1 }) / targetPhysicsFps;
		auto deltaTime = duration<float>{ 0.0f };

		while (isRunning)
		{
			auto timer = high_resolution_clock::now();
			physicsEngine->update(deltaTime.count());
			physicFrameCounter->tick();

			auto time_passed = duration_cast<microseconds>(high_resolution_clock::now() - timer);
			auto time_left = target_delta_time - time_passed;

			if (time_left >= microseconds{ 0 })
			{
				this_thread::sleep_for(time_left);
			}

			deltaTime = (high_resolution_clock::now() - timer);
		}
	}
};