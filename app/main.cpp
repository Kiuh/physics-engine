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
	ivec2 size{ 1200, 1000 };
	string title = "Physics Simulation";
	uint32_t targetPhysicsFps = 60;
	bool isRunning = false;

	WindowProvider* windowProvider;
	GraphicEngine* graphicsEngine;
	DataProvider* dataProvider;
	VertexTransformer* vertexTransformer;
	PhysicsEngine* physicsEngine;

	FpsCounter* graphicFrameCounter;
	FpsCounter* physicFrameCounter;

	thread* graphicThread;
	thread* physicsThread;

	public:
	App()
	{
		this->windowProvider = new WindowProvider(size, title);
		this->vertexTransformer = new VertexTransformer(windowProvider);
		this->dataProvider = new DataProvider(vertexTransformer);
		this->graphicsEngine = new GraphicEngine(windowProvider, dataProvider);
		this->physicsEngine = new PhysicsEngine(dataProvider);

		this->graphicFrameCounter = new FpsCounter("Graphic FPS");
		this->physicFrameCounter = new FpsCounter("Physics FPS");

		graphicThread = nullptr;
		physicsThread = nullptr;
	}

	void run()
	{
		isRunning = true;

		graphicFrameCounter->run();
		physicFrameCounter->run();

		graphicThread = new thread(&App::graphicThreadFunc, this);
		graphicThread->detach();

		physicsThread = new thread(&App::physicsThreadFunc, this);
		physicsThread->detach();

		while (!windowProvider->isShouldClose())
		{
			windowProvider->poolEvents();
		}
		isRunning = false;

		physicsThread->join();
		graphicThread->join();
	}

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

		while (isRunning)
		{
			auto timer = high_resolution_clock::now();
			physicsEngine->update();
			physicFrameCounter->tick();

			auto time_passed = duration_cast<microseconds>(high_resolution_clock::now() - timer);
			auto time_left = target_delta_time - time_passed;

			if (time_left >= microseconds{ 0 })
			{
				this_thread::sleep_for(time_left);
			}
		}
	}

	~App()
	{
		delete this->vertexTransformer;
		delete this->dataProvider;
		delete this->graphicsEngine;
		delete this->windowProvider;
		delete this->physicsEngine;
		delete this->graphicFrameCounter;
	}
};

int main()
{
	App app{};

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}