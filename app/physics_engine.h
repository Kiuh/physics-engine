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

static const glm::vec2 gravity = { 0, -9.8f };

class PhysicsEngine
{
	public:
	std::vector<RigidBody*> rigidBodies;
	std::mutex process_mutex;

	void update(float deltaTime);
	void resolveImpulseCollision(RigidBody& rb1, RigidBody& rb2) const;
	void applyCollisionForce(Collision col, RigidBody& rb1, RigidBody& rb2) const;
};