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
	size_t initialBoxCount = 5;
	size_t initialCircleCount = 7;

	public:
	Controller(WindowManager* win, DataManager* data, PhysicsEngine* engine)
	{
		this->win = win;
		win->keyPressed.connect(boost::bind(&Controller::processKeyPress, this, boost::placeholders::_1));
		this->data = data;
		this->engine = engine;
		setup();
	}

	~Controller()
	{
		cleanup();
	}

	private:
	void setup()
	{
		createGround();
		createBoxes();
		createCircles();

		fillRepresentations();
	}

	void createGround()
	{
		auto tr = new Transform();
		auto plateShape = new Box(*tr);
		plateShape->setHalfSize({ 10, 1.5f });

		auto plate = new Object(plateShape);
		plate->transform->setPos({ 0, -5.0f });
		plate->rigidBody->isStatic = true;
		plate->rigidBody->mass = 20.0f;
		objects.push_back(plate);
	}

	const glm::vec2 getRandomPos()
	{
		static const glm::vec2 leftDown(-10, 0);
		static const glm::vec2 rightUp(10, 7);
		return {
			randomFloat(leftDown.x, rightUp.x),
			randomFloat(leftDown.y, rightUp.y)
		};
	}

	void createBoxes()
	{
		for (size_t i = 0; i < initialBoxCount; i++)
		{
			auto tr = new Transform();
			auto boxShape = new Box(*tr);
			auto box = new Object(boxShape);
			box->transform->setPos(getRandomPos());
			objects.push_back(box);
		}
	}

	void createCircles()
	{
		for (size_t i = 0; i < initialCircleCount; i++)
		{
			auto tr = new Transform();
			auto circleShape = new Circle(*tr);
			auto circle = new Object(circleShape);
			circle->transform->setPos(getRandomPos());
			objects.push_back(circle);
		}
	}

	void fillRepresentations()
	{
		data->data_mutex.lock();
		engine->process_mutex.lock();

		data->dataSources.clear();
		engine->rigidBodies.clear();

		for (size_t i = 0; i < objects.size(); i++)
		{
			data->dataSources.push_back(static_cast<VertexSource*>(objects[i]->shape));
			engine->rigidBodies.push_back(objects[i]->rigidBody);
		}

		engine->process_mutex.unlock();
		data->data_mutex.unlock();
		data->notifyStructureChanging();
	}

	void cleanup()
	{
		for (size_t i = 0; i < objects.size(); i++)
		{
			delete objects[i];
		}
		objects.clear();
	}

	void processKeyPress(KeyCode key)
	{
		if (key == KeyCode::R)
		{
			cleanup();
			setup();
		}
	}
};