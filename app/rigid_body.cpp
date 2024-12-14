#include "rigid_body.h"

RigidBody::RigidBody(Transform& tr, Shape& shape) : tr(tr), shape(shape)
{
	isStatic = false;
	mass = 2.0f;
	restitution = 0.6f;

	area = vt::calcArea(shape.getLocalPoints());
	inertia = calcInertia();
}

void RigidBody::update(float deltaTime)
{
	if (isStatic)
	{
		force = { 0.0f, 0.0f };
		return;
	}

	acceleration = force / mass;
	linearVelocity += acceleration * deltaTime;

	tr.movePos(linearVelocity * deltaTime);
	tr.rotate(angularVelocity * deltaTime);

	force = { 0.0f, 0.0f };
}

void RigidBody::addForce(glm::vec2 amount)
{
	force += amount;
}

void RigidBody::move(glm::vec2 amount)
{
	tr.movePos(amount);
}

float RigidBody::density() const
{
	return mass / area;
}

float RigidBody::invMass() const
{
	return isStatic ? 0.0f : 1.0f / mass;
}

float RigidBody::invInertia() const
{
	return isStatic ? 0.0f : 1.0f / inertia;
}

void RigidBody::drawDebugTooltip() const
{
	ImGui::BeginTooltip();

	ImGui::Text("RigidBody Debug Information:");
	ImGui::Separator();

	ImGui::Text("Position: (%.2f, %.2f)", tr.pos().x, tr.pos().y);
	ImGui::Text("Rotation: %.2f degrees", tr.rot());
	ImGui::Separator();

	ImGui::Text("Linear Velocity: (%.2f, %.2f)", linearVelocity.x, linearVelocity.y);
	ImGui::Text("Acceleration: (%.2f, %.2f)", acceleration.x, acceleration.y);
	ImGui::Text("Angular Velocity: %.2f", angularVelocity);
	ImGui::Separator();

	ImGui::Text("Force: (%.2f, %.2f)", force.x, force.y);
	ImGui::Text("Static: %s", isStatic ? "Yes" : "No");
	ImGui::Text("Mass: %.2f", mass);
	ImGui::Text("Density: %.2f", density());
	ImGui::Text("Inertia: %.2f", inertia);
	ImGui::Text("Restitution: %.2f", restitution);
	ImGui::Text("Area: %.2f", area);

	ImGui::EndTooltip();
}

float RigidBody::calcInertia() const
{
	return vt::calcMomentOfInertia({ 0,0 }, shape.getLocalPoints(), density());
}
