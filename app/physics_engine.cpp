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
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	for (auto& p : gizmo_dots)
	{
		glm::vec2 pos = p;
		pos += dm->zeroShift * glm::vec2{ 1,-1 };
		pos *= dm->pixelsPerUnit;
		pos.y *= -1;
		pos += screenHalfSize;
		drawList->AddCircleFilled({ pos.x, pos.y }, 4.0f, IM_COL32(0, 255, 0, 255));
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

			auto res = Collision2D::tryCollide(rb1.shape, rb2.shape);

			if (res.has_value())
			{
				to_color.insert(&rb1.shape);
				to_color.insert(&rb2.shape);


				auto& col = res.value();
				applyDisplacements(rb1, rb2, col);
				applyImpulses(rb1, rb2, col);

				auto point1 = col.worldContactPointA;
				gizmo_dots.insert(gizmo_dots.end(), point1);
				auto point2 = col.worldContactPointB;
				gizmo_dots.insert(gizmo_dots.end(), point2);
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
	else
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
