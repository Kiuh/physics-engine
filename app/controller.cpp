#pragma once

#include "data_manager.cpp"
#include "physics_engine.cpp"
#include "object.cpp"
#include <limits>

class Controller
{
	private:
	WindowManager* win;
	DataManager* data;
	PhysicsEngine* engine;

	std::vector<Object*> objects{};
	size_t initialBoxCount = 1;

	public:
	Controller(WindowManager* win, DataManager* data, PhysicsEngine* engine)
	{
		this->win = win;
		win->keyPressed.connect(boost::bind(&Controller::processKeyPress, this, boost::placeholders::_1));
		this->data = data;
		this->engine = engine;
		setup();
	}

	private:
	void setup()
	{
		for (size_t i = 0; i < initialBoxCount; i++)
		{
			createBox();
		}

		// Immutable box
		auto plateShape = new AABB();
		plateShape->setHalfSize(glm::vec2{ 10, 1.5f });

		auto plate = new Object();
		plate->transform->setPosition(glm::vec2{ 0, -6.5f });
		plate->rigidBody->inverseMass = std::numeric_limits<float>::infinity();
		plate->setShape(plateShape);
		objects.push_back(plate);

		// Add circle
		auto circleShape = new Circle();

		auto circle = new Object();
		circle->setShape(plateShape);
		objects.push_back(plate);

		data->data_mutex.lock();

		for (size_t i = 0; i < objects.size(); i++)
		{
			data->dataSources.push_back(static_cast<VertexSource*>(objects[i]->shape));
			engine->rigidBodies.push_back(objects[i]->rigidBody);
		}

		data->data_mutex.unlock();
		data->notifyStructureChanging();
	}

	void createBox()
	{
		static const glm::vec2 leftDown(-7, 0);
		static const glm::vec2 rightUp(7, 5);
		static const glm::vec2 halfSize(0.5f, 0.5f);
		static const float mass = 3.0f;

		glm::vec2 position(randomFloat(leftDown.x, rightUp.x), randomFloat(leftDown.y, rightUp.y));

		auto boxShape = new AABB();
		boxShape->setHalfSize(halfSize);

		auto box = new Object();
		box->transform->setPosition(position);
		box->setShape(boxShape);
		objects.push_back(box);
	}

	void processKeyPress(KeyCode key)
	{
		if (key == KeyCode::R)
		{
			//setup();
		}
	}
};