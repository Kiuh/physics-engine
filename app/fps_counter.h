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
	FpsCounter(Debug* debug, std::string prefix);

	void debugUI() const;
	void update(float deltaTime);
};