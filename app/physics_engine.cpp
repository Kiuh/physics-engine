#include "physics_engine.h"

PhysicsEngine::PhysicsEngine(Debug& debug, DataManager& dm) : dm(dm)
{
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
					gizmo_collision_dots.insert(gizmo_collision_dots.end(), col.worldContactPointA);
					gizmo_collision_dots.insert(gizmo_collision_dots.end(), col.worldContactPointB);
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

void PhysicsEngine::applyImpulses(RigidBody& rb1, RigidBody& rb2, Collision col)
{
	auto relativeVelocity = rb2.linearVelocity - rb1.linearVelocity;

	if (glm::dot(relativeVelocity, col.normalBA) > 0.001f)
	{
		return;
	}

	float e = std::fmin(rb1.restitution, rb2.restitution);

	float j = -(1.0f + e) * glm::dot(relativeVelocity, col.normalBA);
	j /= rb1.invMass() + rb2.invMass();

	auto impulse = j * col.normalBA;

	rb1.linearVelocity -= impulse * rb1.invMass();
	rb2.linearVelocity += impulse * rb2.invMass();
}
