#pragma once

#include "debug.h"

#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <string>
#include <thread>

class FpsCounter
{
	private:
	float fps;
	std::string prefix;

	public:
	float shift = 0;

	public:
	FpsCounter(Debug* debug, std::string prefix);

	void debugUI();
	void update(float deltaTime);
};