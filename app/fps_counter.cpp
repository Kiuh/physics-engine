#pragma once

#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <string>
#include <thread>

using namespace std;
using namespace chrono;

class FpsCounter
{
	private:
	uint32_t fps;
	string prefix;
	thread* fpsThread;
	bool isRunning;

	public:
	FpsCounter(string prefix)
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
		fpsThread = new thread(&FpsCounter::printInInterval, this);
		fpsThread->detach();
	}

	void printInInterval()
	{
		while (isRunning)
		{
			this_thread::sleep_for(seconds(1));

			stringstream msg;
			msg << prefix << ": " << fps << endl;
			cout << msg.str();
			fps = 0;
		}
	}

	void tick()
	{
		fps++;
	}
};