#pragma once

#include "collision.h"
#include "collision_tools.h"
#include "data_manager.h"
#include "object.h"
#include "random_helper.h"
#include "rigid_body.h"
#include "window_manager.h"

#include <boost/bind/bind.hpp>
#include <iostream>
#include <set>
#include <vector>

class PhysicsEngine
{
	public:
	std::vector<RigidBody*> rigidBodies;
	std::mutex process_mutex;

	void update(float deltaTime);

	private:
	void resolveCollision(RigidBody& rb1, RigidBody& rb2, Collision col);
};