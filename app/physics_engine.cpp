#include "physics_engine.h"

PhysicsEngine::PhysicsEngine(Debug* debug, DataManager* dm)
{
	this->dm = dm;
	debug->buildUI.connect(boost::bind(&PhysicsEngine::buildDebugUI, this));
}

void PhysicsEngine::update(float deltaTime)
{
	process_mutex.lock();

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

	// Collisions
	resolveCollisions();

	process_mutex.unlock();
}

void DrawDotExample(ImVec2 screenPos, float radius, ImU32 color)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddCircleFilled(screenPos, radius, color);
}

void DrawDotOverlay(ImVec2 screenPos, float radius, ImU32 color)
{
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddCircleFilled(screenPos, radius, color);
}

void PhysicsEngine::buildDebugUI()
{
	ImGui::Begin("Physics parameters", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat2("Gravity", &gravity.x, 0.05f, -15.0f, 15.0f, "%.2f");
	ImGui::End();

	ImGui::SetNextWindowBgAlpha(0);
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("Dots", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

	gizmo_mutex.lock();
	glm::vec2 screenHalfSize{ ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f };
	for (auto& p : gizmo_dots)
	{
		glm::vec2 pos = p;
		pos += dm->zeroShift * glm::vec2{ 1,-1 };
		pos *= dm->pixelsPerUnit;
		pos.y *= -1;
		pos += screenHalfSize;
		DrawDotOverlay({ pos.x, pos.y }, 4.0f, IM_COL32(0, 255, 0, 255));
	}
	gizmo_mutex.unlock();

	ImGui::End();
}

void PhysicsEngine::resolveCollisions()
{
	std::set<Shape*> to_color{};

	gizmo_mutex.lock();
	gizmo_dots.clear();

	// Collision
	for (size_t i = 0; i < rigidBodies.size(); i++)
	{
		for (size_t j = i + 1; j < rigidBodies.size(); j++)
		{
			auto& rb1 = *rigidBodies[i];
			auto& rb2 = *rigidBodies[j];

			if (rb1.isStatic && rb2.isStatic) continue;

			if (isOverlaps(rb1.shape, rb2.shape))
			{
				to_color.insert(&rb1.shape);
				to_color.insert(&rb2.shape);

				auto res1 = getCollision(rb1.shape, rb2.shape);
				if (res1.has_value())
				{
					auto& col = res1.value();
					applyDisplacements(rb1, rb2, col);
					applyImpulses(rb1, rb2, col);
				}

				auto res2 = getWorldContactPoints(rb1.shape, rb2.shape);
				if (res2.has_value())
				{
					auto& list = res2.value();
					gizmo_dots.insert(gizmo_dots.end(), list.begin(), list.end());
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
		rb2.move(col.vec());
	}
	else if (rb2.isStatic)
	{
		rb1.move(-col.vec());
	}
	else
	{
		rb1.move(-col.vec() / 2.0f);
		rb2.move(col.vec() / 2.0f);
	}
}

void PhysicsEngine::applyImpulses(RigidBody& rb1, RigidBody& rb2, Collision col)
{
	auto relativeVelocity = rb2.linearVelocity - rb1.linearVelocity;

	if (glm::dot(relativeVelocity, col.normal) > 0.001f)
	{
		return;
	}

	float e = std::fmin(rb1.restitution, rb2.restitution);

	float j = -(1.0f + e) * glm::dot(relativeVelocity, col.normal);
	j /= rb1.invMass() + rb2.invMass();

	auto impulse = j * col.normal;

	rb1.linearVelocity -= impulse * rb1.invMass();
	rb2.linearVelocity += impulse * rb2.invMass();
}
