#pragma once

#include "data_provider.cpp"

class PhysicsEngine
{
	private:
	DataProvider* dp;

	public:
	PhysicsEngine(DataProvider* dp)
	{
		this->dp = dp;
	}

	void update()
	{
	}
};