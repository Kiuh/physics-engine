#pragma once

#include "collision.h"
#include "collision_2d.h"
#include "data_manager.h"
#include "debug.h"
#include "imgui_helper.h"
#include "object.h"
#include "random_helper.h"
#include "rigid_body.h"
#include "window_manager.h"

#include <boost/bind/bind.hpp>
#include <imgui.h>
#include <iostream>
#include <set>
#include <vector>

class PhysicsEngine
{
	private:
	DataManager& dm;

	public:
	std::vector<RigidBody*> rigidBodies;
	std::mutex process_mutex;
	glm::vec2 gravity;

	std::mutex gizmo_mutex{};
	std::vector<glm::vec2> gizmo_collision_dots{};
	std::vector<std::vector<glm::vec2>> gizmo_collision_mink_hulls{};
	std::vector<std::vector<glm::vec2>> gizmo_collision_mink_final_triangles{};
	std::vector<std::vector<glm::vec2>> gizmo_collision_mink_tangents{};
	std::vector<std::vector<glm::vec2>> gizmo_collision_mink_diff_triangles{};

	bool simulate;
	bool draw_contact_points;
	bool draw_collision_gizmo;
	bool color_on_collision;

	PhysicsEngine(Debug& debug, DataManager& dm);
	void update(float deltaTime);
	void buildDebugUI();

	private:
	void resolveCollisions();
	void applyDisplacements(RigidBody& rb1, RigidBody& rb2, Collision col);
	void applyImpulses(RigidBody& rb1, RigidBody& rb2, Collision col);
};