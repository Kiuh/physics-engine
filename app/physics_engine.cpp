#include "physics_engine.h"

PhysicsEngine::PhysicsEngine(Debug& debug, DataManager& dm) : dm(dm)
{
	gravity = { 0,-9.8f };

	simulate = true;
	draw_contact_points = true;
	draw_collision_gizmo = false;

	debug.buildUI.connect(boost::bind(&PhysicsEngine::buildDebugUI, this));
}

void PhysicsEngine::update(float deltaTime)
{
	process_mutex.lock();

	if (simulate)
	{
		// Forces
		for (auto rb : rigidBodies)
		{
			rb->addForce(gravity);
		}

		// Update positions
		for (auto rb : rigidBodies)
		{
			rb->update(deltaTime);
		}
	}

	// Collisions
	resolveCollisions();

	process_mutex.unlock();
}

void PhysicsEngine::buildDebugUI()
{
	ImGui::Begin("Physics parameters", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat2("Gravity", &gravity.x, 0.05f, -15.0f, 15.0f, "%.2f");
	ImGui::Checkbox("Simulate", &simulate);
	ImGui::Checkbox("Draw collision gizmo", &draw_collision_gizmo);
	ImGui::Checkbox("Draw contact points", &draw_contact_points);
	ImGui::End();

	ImGui::SetNextWindowBgAlpha(0);
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("Dots", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

	gizmo_mutex.lock();
	drawDots(dm, gizmo_collision_dots);
	drawConvexHulls(dm, gizmo_collision_mink_hulls);
	drawConvexHulls(dm, gizmo_collision_mink_final_triangles, Color::white());
	drawConvexHulls(dm, gizmo_collision_mink_diff_triangles, Color::yellow(), 1.0f);
	drawLineSegments(dm, gizmo_collision_mink_tangents, Color::green());
	gizmo_mutex.unlock();

	ImGui::End();
}

void PhysicsEngine::resolveCollisions()
{
	std::set<Shape*> to_color{};

	gizmo_mutex.lock();
	gizmo_collision_dots.clear();
	gizmo_collision_mink_hulls.clear();
	gizmo_collision_mink_final_triangles.clear();
	gizmo_collision_mink_diff_triangles.clear();
	gizmo_collision_mink_tangents.clear();

	// Collision
	for (size_t i = 0; i < rigidBodies.size(); i++)
	{
		for (size_t j = i + 1; j < rigidBodies.size(); j++)
		{
			auto& rb1 = *rigidBodies[i];
			auto& rb2 = *rigidBodies[j];

			auto res = Collision2D::tryCollide(rb1.shape, rb2.shape);

			if (res.has_value())
			{
				auto& col = res.value();

				to_color.insert(&rb1.shape);
				to_color.insert(&rb2.shape);

				if (simulate)
				{
					applyDisplacements(rb1, rb2, col);
					applyImpulses(rb1, rb2, col);
				}

				if (draw_collision_gizmo)
				{
					gizmo_collision_mink_final_triangles.insert(
						gizmo_collision_mink_final_triangles.end(),
						col.gizmo_finalTriangle
					);
					gizmo_collision_mink_hulls.insert(
						gizmo_collision_mink_hulls.end(),
						col.gizmo_minkConvex
					);
					gizmo_collision_mink_tangents.insert(
						gizmo_collision_mink_tangents.end(),
						col.gizmo_minkTangent
					);
					gizmo_collision_mink_diff_triangles.insert(
						gizmo_collision_mink_diff_triangles.end(),
						col.gizmo_diff_triangles.begin(),
						col.gizmo_diff_triangles.end()
					);
				}

				if (draw_contact_points)
				{
					gizmo_collision_dots.insert(gizmo_collision_dots.end(), col.worldContactPoint);
				}
			}
		}
	}

	gizmo_mutex.unlock();

	for (auto rb : rigidBodies)
	{
		rb->shape.color = Color::white();
	}

	for (auto sh : to_color)
	{
		sh->color = Color::red();
	}
}

void PhysicsEngine::applyDisplacements(RigidBody& rb1, RigidBody& rb2, Collision col)
{
	if (rb1.isStatic)
	{
		rb2.move(col.getNormalVecBA());
	}
	else if (rb2.isStatic)
	{
		rb1.move(col.getNormalVecAB());
	}
	else if (!rb1.isStatic && !rb2.isStatic)
	{
		rb1.move(col.getNormalVecAB() / 2.0f);
		rb2.move(col.getNormalVecBA() / 2.0f);
	}
}

void PhysicsEngine::applyImpulses(RigidBody& rbA, RigidBody& rbB, Collision col)
{
	auto normal = col.normalBA;
	auto ra = col.localContactPointA;
	auto rb = col.localContactPointB;

	glm::vec2 raPerp{ -ra.y, ra.x };
	glm::vec2 rbPerp{ -rb.y, rb.x };

	auto angularLinearVelocityA = raPerp * rbA.angularVelocity;
	auto angularLinearVelocityB = rbPerp * rbB.angularVelocity;

	auto relativeVelocity =
		(rbB.linearVelocity + angularLinearVelocityB) -
		(rbA.linearVelocity + angularLinearVelocityA);

	float contactVelocityMag = glm::dot(relativeVelocity, normal);
	if (contactVelocityMag > 0.001f)
	{
		return;
	}

	float raPerpDotN = glm::dot(raPerp, normal);
	float rbPerpDotN = glm::dot(rbPerp, normal);

	float denom = rbA.invMass() + rbB.invMass() +
		(raPerpDotN * raPerpDotN) * rbA.invInertia() +
		(rbPerpDotN * rbPerpDotN) * rbB.invInertia();

	float e = std::fmin(rbA.restitution, rbB.restitution);
	float j = -(1.0f + e) * contactVelocityMag;
	j /= denom;

	auto impulse = j * normal;

	rbA.linearVelocity += -impulse * rbA.invMass();
	rbA.angularVelocity += -vt::cross(ra, impulse) * rbA.invInertia();
	rbB.linearVelocity += impulse * rbB.invMass();
	rbB.angularVelocity += vt::cross(rb, impulse) * rbB.invInertia();
}
