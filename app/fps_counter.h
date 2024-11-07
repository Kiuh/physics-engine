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
	FpsCounter(std::string prefix);
	~FpsCounter();

	void run();
	void stop();
	void printInInterval();
	void tick();
};