#include "controller.h"

Controller::Controller(Debug* debug, WindowManager* win, DataManager* data, PhysicsEngine* engine)
{
	initialBoxCount = 1;
	initialCircleCount = 1;
	initialPolygonsCount = 1;

	snapToPointer = false;
	snapIndex = 1;
	showRigidBodyTooltip = true;

	this->win = win;
	win->keyPressed.connect(boost::bind(&Controller::processKeyPress, this, boost::placeholders::_1));
	win->mouseMoved.connect(boost::bind(&Controller::mouseMoved, this, boost::placeholders::_1));
	win->mouseButtonPressed.connect(boost::bind(&Controller::mouseButton, this, boost::placeholders::_1));
	debug->buildUI.connect(boost::bind(&Controller::debugUI, this));
	this->data = data;
	this->engine = engine;
	setup();
}

Controller::~Controller()
{
	cleanup();
}

void Controller::setup()
{
	createGround();
	createPolygons();
	createBoxes();
	createCircles();

	fillRepresentations();
}

bool Controller::isNoIntersections(Object& obj)
{
	for (auto ob : objects)
	{
		if (Collision2D::isOverlaps(*obj.shape, *ob->shape))
		{
			return false;
		}
	}
	return true;
}

void Controller::createGround()
{
	auto tr = new Transform();
	auto& plateShape = createBoxShape(*tr, { 10, 1.5f });

	auto plate = new Object(plateShape);
	plate->tr->setPos({ 0, -5.0f });
	plate->rigidBody->isStatic = true;
	plate->rigidBody->mass = 20.0f;
	objects.push_back(plate);
}

const glm::vec2 Controller::getRandomPos()
{
	static const glm::vec2 leftDown(-10, 0);
	static const glm::vec2 rightUp(10, 7);
	return {
		randomFloat(leftDown.x, rightUp.x),
		randomFloat(leftDown.y, rightUp.y)
	};
}

void Controller::createAndPlace(Shape& shape)
{
	auto obj = new Object(shape);
	do
	{
		obj->tr->setPos(getRandomPos());
	}
	while (!isNoIntersections(*obj));
	obj->tr->setRot(randomFloat(0, 360));
	objects.push_back(obj);
}

void Controller::createBoxes()
{
	for (size_t i = 0; i < initialBoxCount; i++)
	{
		auto tr = new Transform();
		auto& boxShape = createBoxShape(*tr, { 1.0f, 1.0f });
		createAndPlace(boxShape);
	}
}

void Controller::createCircles()
{
	for (size_t i = 0; i < initialCircleCount; i++)
	{
		auto tr = new Transform();
		auto& circleShape = createCircleShape(*tr, 1.0f);
		createAndPlace(circleShape);
	}
}

void Controller::createPolygons()
{
	for (size_t i = 0; i < initialPolygonsCount; i++)
	{
		auto tr = new Transform();
		auto& shape = createRandomPolygonShape(*tr, 5, 1.5f);
		createAndPlace(shape);
	}
}

void Controller::fillRepresentations()
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

void Controller::cleanup()
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();
}

void Controller::processKeyPress(KeyCode key)
{
	if (key == KeyCode::R)
	{
		cleanup();
		setup();
	}
}

void Controller::mouseMoved(glm::vec2 pos)
{
	if (snapToPointer && snapIndex < objects.size())
	{
		objects[snapIndex]->tr->setPos(data->screenCoordToWorld(pos));
	}
}

void Controller::mouseButton(MouseButton mouseButton)
{
	if (mouseButton == MouseButton::Right && snapToPointer)
	{
		snapToPointer = false;
	}
}

void Controller::addPolygon()
{
	data->data_mutex.lock();
	engine->process_mutex.lock();

	auto tr = new Transform();
	auto& shape = createRandomPolygonShape(*tr, 5, 1.5f);
	createAndPlace(shape);

	data->dataSources.push_back(static_cast<VertexSource*>(objects.back()->shape));
	engine->rigidBodies.push_back(objects.back()->rigidBody);

	engine->process_mutex.unlock();
	data->data_mutex.unlock();

	data->notifyStructureChanging();
}

void Controller::debugUI()
{
	ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Add Polygon"))
	{
		addPolygon();
	}
	ImGui::Checkbox("Snap to pointer", &snapToPointer);
	ImGui::Checkbox("Show RigidBody tooltip", &showRigidBodyTooltip);
	ImGui::SliderInt("Snap Index", &snapIndex, 0, static_cast<int>(objects.size()) - 1);
	if (ImGui::Button("Refresh simulation"))
	{
		cleanup();
		setup();
	}
	if (showRigidBodyTooltip)
	{
		auto mPos = data->screenCoordToWorld(win->getMousePos());
		for (auto& obj : objects)
		{
			if (vt::isPointInsideConvexPolygon(mPos, obj->shape->getWorldPoints()))
			{
				obj->rigidBody->drawDebugTooltip();
				break;
			}
		}
	}
	ImGui::End();
}
