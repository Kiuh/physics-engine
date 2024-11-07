#include "fps_counter.h"

FpsCounter::FpsCounter(std::string prefix)
{
	this->prefix = prefix;
	fps = 0;
	fpsThread = nullptr;
	isRunning = false;
}

FpsCounter::~FpsCounter()
{
	isRunning = false;
}

void FpsCounter::run()
{
	isRunning = true;
	fpsThread = new std::thread(&FpsCounter::printInInterval, this);
}

void FpsCounter::stop()
{
	isRunning = false;
	fpsThread->join();
}

void FpsCounter::printInInterval()
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

void FpsCounter::tick()
{
	fps++;
}