#pragma once

#include "data_manager.h"
#include "random_helper.h"
#include "window_manager.h"
#include "object.h"
#include "rigid_body.h"
#include "detectors.h"
#include "resolvers.h"

#include <boost/bind/bind.hpp>
#include <iostream>
#include <vector>
#include <set>

class PhysicsEngine
{
	public:
	std::vector<RigidBody*> rigidBodies;
	std::mutex process_mutex;

	void update(float deltaTime);
};