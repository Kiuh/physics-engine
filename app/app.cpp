#include "app.h"

App::App()
{
	graphicsEngineConfig = GraphicsEngineConfig{};
#ifdef NDEBUG
	graphicsEngineConfig.validation = false;
#else
	graphicsEngineConfig.validation = true;
#endif
	graphicsEngineConfig.maxFramesInFlight = 2;

	debug = std::make_unique<Debug>();

	windowManager = std::make_unique<WindowManager>(size, title);
	dataManager = std::make_unique<DataManager>(*windowManager.get());

	physicsEngine = std::make_unique<PhysicsEngine>(debug.get(), dataManager.get());
	graphicsEngine = std::make_unique<GraphicEngine>(debug.get(), windowManager.get(), dataManager.get(), graphicsEngineConfig);

	controller = std::make_unique<Controller>(debug.get(), windowManager.get(), dataManager.get(), physicsEngine.get());

	graphicFrameCounter = std::make_unique<FpsCounter>(debug.get(), "Graphic: ");
	physicFrameCounter = std::make_unique<FpsCounter>(debug.get(), "Physics: ");

	windowManager->keyPressed.connect(boost::bind(&App::processKeyPress, this, boost::placeholders::_1));
}

void App::run()
{
	isRunning = true;

	graphicThread = std::make_unique<std::thread>(&App::graphicThreadFunc, this);
	physicsThread = std::make_unique<std::thread>(&App::physicsThreadFunc, this);

	while (!windowManager->isShouldClose() && isRunning)
	{
		windowManager->poolEvents();
	}
	isRunning = false;

	physicsThread->join();
	graphicThread->join();
}


void App::processKeyPress(KeyCode key)
{
	if (key == ESC)
	{
		isRunning = false;
	}
}

void App::graphicThreadFunc()
{
	auto deltaTime = std::chrono::duration<float>{ 0.0f };
	while (isRunning)
	{
		auto timer = std::chrono::high_resolution_clock::now();
		graphicsEngine->draw();

		deltaTime = (std::chrono::high_resolution_clock::now() - timer);
		graphicFrameCounter->update(deltaTime.count());
	}
}

void App::physicsThreadFunc()
{
	const auto target_delta_time = duration_cast<std::chrono::microseconds>(std::chrono::seconds{ 1 }) / targetPhysicsFps;
	auto deltaTime = std::chrono::duration<float>{ 0.0f };

	const std::chrono::duration<float> delay = std::chrono::duration<float>{ 0.00f };

	while (isRunning)
	{
		auto timer = std::chrono::high_resolution_clock::now();
		physicsEngine->update(deltaTime.count());
		physicFrameCounter->update(deltaTime.count());

		auto time_passed = duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer);
		auto time_left = target_delta_time - time_passed;

		if (time_left >= std::chrono::microseconds{ 0 })
		{
			std::this_thread::sleep_for(time_left);
		}

		deltaTime = (std::chrono::high_resolution_clock::now() - timer);

		std::this_thread::sleep_for(delay);
	}
}
