#pragma once

#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <string>
#include <thread>

class FpsCounter
{
	private:
	uint32_t fps;
	std::string prefix;
	std::thread* fpsThread;
	bool isRunning;

	public:
	FpsCounter(std::string prefix)
	{
		this->prefix = prefix;
		fps = 0;
		fpsThread = nullptr;
		isRunning = false;
	}

	~FpsCounter()
	{
		isRunning = false;
	}

	void run()
	{
		isRunning = true;
		fpsThread = new std::thread(&FpsCounter::printInInterval, this);
		fpsThread->detach();
	}

	void printInInterval()
	{
		while (isRunning)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));

			std::stringstream msg;
			msg << prefix << ": " << fps << std::endl;
			std::cout << msg.str();
			fps = 0;
		}
	}

	void tick()
	{
		fps++;
	}
};