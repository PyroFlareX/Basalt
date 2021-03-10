#include "Gamestate.h"


GameState::GameState(Application &app) : Basestate(app) {
	app.getCam().follow(m_player);

	//Original Code For Setting Positions
	/*glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-2.0f, 2.0f, -3.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(2.0f, 1.0f, -4.0f),
		glm::vec3(-2.0f,  3.0f, -8.0f),
		glm::vec3(1.0f, 2.0f, -3.0f),
		glm::vec3(2.0f,  2.0f, -3.0f),
		glm::vec3(2.0f,  0.0f, -2.0f),
		glm::vec3(-1.0f,  1.0f, -2.0f)
	};
	for (int i = 0; i < 10; ++i)
	{
		vn::Transform entity;

		entity.pos = vn::vec3(1.0f, 0.0f, 1.0f);
		entity.pos = cubePositions[i];
		entity.rescale(entity, vn::vec3(0.5f, 0.5f, 0.5f));
		//entity.rotate(entity, vn::vec3(0, 360 * sin(x++ / 10000.0), 0));

		btCollisionShape* shape = new btSphereShape(1.0f);

		vn::GameObject gameobject(entity, shape);

		m_gameObjects.emplace_back(gameobject);
		m_world.addObject(gameobject);
	}
	//vn::saveStateToFile("res/Levels/Test/Test.json", m_gameObjects);*/

	//Temporary 


	//Loads GameObjects
	//vn::loadStateFromFile("res/Levels/Test/Test.json", m_gameObjects);
	//for (auto& gobj : m_gameObjects)
	//{
	//	m_world.addObject(gobj);
	//}
	vn::Transform t;
	vn::GameObject gobj(t);

	m_gameObjects.emplace_back(gobj);
}

GameState::~GameState() {
	//res/Levels/TestEnd/TestEnd.json
	//vn::saveStateToFile("res/Levels/TestEnd/TestEnd.json", m_gameObjects);
}

bool GameState::input() {

	vInput = Input::getInput();

	m_player.getInput(vInput, false);


	return false;
}

void GameState::update(float dt) {
	void *data[] = {&dt, &m_player, nullptr, &m_gameObjects};

	//m_player.update(dt);

	// Player and World updates as a Job
	Job update = jobSystem.createJob([](Job job) {
		float deltatime = *static_cast<float *>(job.data[0]);

		static_cast<Player *>(job.data[1])->update(deltatime);
		//m_world.update(dt);

	}, data);

	// Updates for all objects
	Job objUpdates = jobSystem.createJob([](Job job) {
		for (auto &obj : *static_cast<std::vector<vn::GameObject> *>(job.data[3])) {
			void *objData[] = {&obj};
			// A job for updating each individual object
			Job doObjectUpdate = jobSystem.createJob([](Job jobObj) {
				static_cast<vn::GameObject *>(jobObj.data[0])->update();
			}, objData);
			jobSystem.schedule(doObjectUpdate);
		}
	}, data);

	// Schedule the jobs
	jobSystem.schedule(update);
	//jobSystem.schedule(objUpdates);

	jobSystem.wait();

}

void GameState::lateUpdate(Camera *cam) {

}

void GameState::render(Renderer *renderer) {
	for (auto &obj : m_gameObjects) {
		obj.getCurrentTransform();
		renderer->drawObject(obj);
	}
}

