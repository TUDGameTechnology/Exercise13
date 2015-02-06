#include "pch.h"

#include <Kore/Application.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/KeyEvent.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Audio/Mixer.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>
#include <Kore/Math/Random.h>

#include "MeshObject.h"
#include "Steering.h"
#include "Flocking.h"
#include "StateMachine.h"
#include <Kore/Log.h>

using namespace Kore;













namespace {
	const int width = 1024;
	const int height = 768;
	const int numBoids = 20;
	double startTime;
	Shader* vertexShader;
	Shader* fragmentShader;
	Program* program;

	// null terminated array of MeshObject pointers
	MeshObject* objects[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	// The view projection matrix aka the camera
	mat4 P;
	mat4 View;

	// uniform locations - add more as you see fit
	TextureUnit tex;
	ConstantLocation pLocation;
	ConstantLocation vLocation;
	ConstantLocation mLocation;

	float angle;

	vec2 EarthPosition;
	vec2 deltaPosition;

	Wander* wander;
	Seek* seek;

	AICharacter* moon;
	AICharacter* earth;

	SteeringOutput playerSteering;

	StateMachine moonStateMachine;


	AICharacter* boids[20];

	/** Holds the flock */
	Flock flock;

	BlendedSteering* flockSteering;

	/** Holds the steering behaviours. */
	Separation *separation;
	Cohesion *cohesion;
	VelocityMatchAndAlign *vMA;


	SteeringBehaviour* moonBehaviour;

	enum State { Wandering, Following };


	class MoonAction : public Action {
	public:
		State state;

		virtual void act() {
			if (state == Following) {
				moonBehaviour = seek;
				Kore::log(Kore::Info, "Moon is now seeking");
			}
			else {
				moonBehaviour = wander;
				Kore::log(Kore::Info, "Moon is now wandering");
			}
		}
	};

	class MoonState : public StateMachineState
	{
	public:

		State state;

		virtual Action* getEntryActions() {
			MoonAction* changeStatusAction = new MoonAction();
			changeStatusAction->state = state;
			changeStatusAction->next = nullptr;
			return changeStatusAction;
		}

		virtual Action* getExitActions() {
			return new Action();
		}


	};


	class MoonTransition :
		public Transition,
		public ConditionalTransitionMixin,
		public FixedTargetTransitionMixin
	{
	public:

		virtual Action* getActions() {
			return new Action();
		}

		virtual bool isTriggered() {
			bool result = ConditionalTransitionMixin::isTriggered();
			return result;
		}

		StateMachineState* getTargetState()
		{
			return FixedTargetTransitionMixin::getTargetState();
		}
	};

	class MoonCondition : public Condition {
	public:


		float transitionDistance;

		bool checkIfCloser;

		AICharacter* earthCharacter;

		AICharacter* moonCharacter;

		bool lastResult;
		/**
		* Performs the test for this condition.
		*/
		virtual bool test() {
			float distance = earthCharacter->Position.distance(moonCharacter->Position);
			bool result;
			

			if (checkIfCloser) {
				result = (distance < transitionDistance);
				if (result != lastResult) {
					if (checkIfCloser) {
						Kore::log(Kore::Info, "checkIfCloser TRUE:");
					}
					else {
						Kore::log(Kore::Info, "checkIfCloser FALSE:");
					}
					if (result) {
						Kore::log(Kore::Info, "Moon is closer than distance");
					}
					else {
						Kore::log(Kore::Info, "Moon is further than distance");
					}
				}
			}
			else {
				result = (distance >= transitionDistance);
				if (result != lastResult) {
					if (checkIfCloser) {
						Kore::log(Kore::Info, "checkIfCloser TRUE:");
					}
					else {
						Kore::log(Kore::Info, "checkIfCloser FALSE:");
					}
					if (result) {
						Kore::log(Kore::Info, "Moon is further than distance");
					}
					else {
						Kore::log(Kore::Info, "Moon is closer than distance");
					}
				}
			}

			lastResult = result;

			return result;
		}
	};


	void updateAI() {
		// Update the state machine
		Action* actions = moonStateMachine.update();
		while (actions != nullptr) {
			actions->act();
			actions = actions->next;
		} 


		// TODO: Compute
		float duration = 0.05f;
		SteeringOutput steer;

		moonBehaviour->getSteering(&steer);

		moon->integrate(steer, 0.95f, duration);

		// moon->setOrientationFromVelocity();

		moon->trimMaxSpeed(0.2f);


		playerSteering.linear = deltaPosition;
		earth->integrate(playerSteering, 0.95f, duration);
		
		earth->trimMaxSpeed(0.3f);
		


#define WORLD_SIZE 2.0f

#define TRIM_WORLD(var) \
		if (var < -WORLD_SIZE) var = WORLD_SIZE; \
		if (var > WORLD_SIZE) var = -WORLD_SIZE;

		// Keep in bounds of the world
		TRIM_WORLD(moon->Position[0]);
		TRIM_WORLD(moon->Position[1]);

		// Keep in bounds of the world
		TRIM_WORLD(earth->Position[0]);
		TRIM_WORLD(earth->Position[1]);


		moon->meshObject->M = mat4::Translation(moon->Position[0], 0.0f, moon->Position[1]);
		earth->meshObject->M = mat4::Translation(earth->Position[0], 0.0f, earth->Position[1]);



		SteeringOutput temp;

		for (int i = 0; i < numBoids; i++) {
			AICharacter* boid = boids[i];

			// Get the steering output
			flockSteering->character = boid;
			flockSteering->getSteering(&steer);

			// Update the kinematic
			boid->integrate(steer, 0.7f, duration);
			// TODO: ADD
			boid->setOrientationFromVelocity();

			// Check for maximum speed
			boid->trimMaxSpeed(4.0f);

			// Keep in bounds of the world
			TRIM_WORLD(boid->Position[0]);
			TRIM_WORLD(boid->Position[1]);

			boid->meshObject->M = mat4::Translation(boid->Position[0], 0.0f, boid->Position[1]) * mat4::RotationY(boid->Orientation + Kore::pi);
		}

	}





	void update() {
		float t = (float)(System::time() - startTime);
		Kore::Audio::update();

		Graphics::begin();
		Graphics::clear(Graphics::ClearColorFlag | Graphics::ClearDepthFlag, 0xff9999FF, 1000.0f);

		program->set();

		updateAI();

		


		// set the camera
		P = mat4::Perspective(60, (float)width / (float)height, 0.1f, 100);
		//float val = 1.0f;
		//P = mat4::orthogonalProjection(-val, val, -val, val, -val, val);
		View = mat4::lookAt(vec3(0, 4, 0), vec3(0, 0, 0), vec3(0, 0, 1.0f));
		//V = mat4::Identity();
		Graphics::setMatrix(pLocation, P);
		Graphics::setMatrix(vLocation, View);


		angle = t;





		// iterate the MeshObjects
		MeshObject** current = &objects[0];
		while (*current != nullptr) {
			// set the model matrix
			Graphics::setMatrix(mLocation, (*current)->M);

			(*current)->render(tex);
			++current;
		}

		Graphics::end();
		Graphics::swapBuffers();
	}





	void mouseMove(int x, int y) {

	}

	void mousePress(int button, int x, int y) {

	}

	void mouseRelease(int button, int x, int y) {

	}



	void keyDown(KeyEvent* event) {

		float movementDelta = 0.1f;

		if (event->keycode() == Key_Left) {
			deltaPosition[0] = -movementDelta;
		}
		else if (event->keycode() == Key_Right) {
			deltaPosition[0] = movementDelta;
		}
		else if (event->keycode() == Key_Up) {
			deltaPosition[1] = movementDelta;
		}
		else if (event->keycode() == Key_Down) {
			deltaPosition[1] = -movementDelta;
		}
	}

	void keyUp(KeyEvent* event) {


		if (event->keycode() == Key_Left) {
			deltaPosition[0] = 0.0f;
		}
		else if (event->keycode() == Key_Right) {
			deltaPosition[0] = 0.0f;
		}
		else if (event->keycode() == Key_Up) {
			deltaPosition[1] = 0.0f;
		}
		else if (event->keycode() == Key_Down) {
			deltaPosition[1] = 0.0f;
		}
	}




	void init() {
		FileReader vs("shader.vert");
		FileReader fs("shader.frag");
		vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
		fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

		// This defines the structure of your Vertex Buffer
		VertexStructure structure;
		structure.add("pos", Float3VertexData);
		structure.add("tex", Float2VertexData);
		structure.add("nor", Float3VertexData);

		program = new Program;
		program->setVertexShader(vertexShader);
		program->setFragmentShader(fragmentShader);
		program->link(structure);

		tex = program->getTextureUnit("tex");
		pLocation = program->getConstantLocation("P");
		vLocation = program->getConstantLocation("V");
		mLocation = program->getConstantLocation("M");

		objects[0] = new MeshObject("Level/ball.obj", "Level/unshaded.png", structure);
		objects[0]->M = mat4::Identity();
		objects[1] = new MeshObject("Level/plane.obj", "Level/StarMap.png", structure);
		objects[2] = new MeshObject("Level/ball.obj", "Level/moonmap1k.jpg", structure);
		objects[3] = new MeshObject("Level/boid.obj", "Level/basicTiles3x3red.png", structure);
		objects[3]->M = mat4::Scale(0.1f, 0.1f, 0.1f);

		moon = new AICharacter();
		moon->Position = vec2(1.0f, 1.0f);
		moon->meshObject = objects[2];

		earth = new AICharacter();
		earth->meshObject = objects[0];




		for (int i = 0; i < numBoids; i++) {
			objects[3 + i] = new MeshObject("Level/boid.obj", "Level/basicTiles3x3red.png", structure);
			boids[i] = new AICharacter();
			AICharacter* current = boids[i];
			current->meshObject = objects[3 + i];
			current->Position[0] = Wander::randomBinomial(WORLD_SIZE);
			current->Position[1] = Wander::randomBinomial(WORLD_SIZE);
			current->Orientation = Wander::randomReal(Kore::pi);
			current->Velocity[0] = Wander::randomBinomial(2.0f);
			current->Velocity[1] = Wander::randomReal(2.0f);
			current->Rotation = 0.0f;
			flock.boids.push_back(current);
		}


		wander = new Wander();
		wander->character = moon;
		wander->maxAcceleration = 0.2f;
		wander->turnSpeed = 2.0f;
		wander->volatility = 20.0f;

		seek = new Seek();
		seek->character = moon;
		seek->maxAcceleration = 0.2f;
		seek->target = &(earth->Position);

		moonBehaviour = wander;

		float accel = 0.5f;
		// Set up the steering behaviours (we use one for all)
		separation = new Separation;
		separation->maxAcceleration = accel;
		separation->neighbourhoodSize = 5.0f;
		separation->neighbourhoodMinDP = -1.0f;
		separation->theFlock = &flock;

		cohesion = new Cohesion;
		cohesion->maxAcceleration = accel;
		cohesion->neighbourhoodSize = 10.0f;
		cohesion->neighbourhoodMinDP = 0.0f;
		cohesion->theFlock = &flock;

		vMA = new VelocityMatchAndAlign;
		vMA->maxAcceleration = accel;
		vMA->neighbourhoodSize = 15.0f;
		vMA->neighbourhoodMinDP = 0.0f;
		vMA->theFlock = &flock;


		flockSteering = new BlendedSteering();
		flockSteering->behaviours.push_back(BlendedSteering::BehaviourAndWeight(
			separation, 1.0f
			));
		flockSteering->behaviours.push_back(BlendedSteering::BehaviourAndWeight(
			cohesion, 1.0f
			));
		flockSteering->behaviours.push_back(BlendedSteering::BehaviourAndWeight(
			vMA, 2.0f
			));



		MoonState* wanderState = new MoonState();
		MoonState* followState = new MoonState();
		wanderState->state = Wandering;
		followState->state = Following;

		MoonTransition* WanderingToFollowing = new MoonTransition();
		WanderingToFollowing->target = followState;

		MoonTransition* FollowingToWandering = new MoonTransition();
		FollowingToWandering->target = wanderState;

		MoonCondition* ShouldFollow = new MoonCondition();
		ShouldFollow->checkIfCloser = true;
		ShouldFollow->earthCharacter = earth;
		ShouldFollow->moonCharacter = moon;
		ShouldFollow->transitionDistance = 1.0f;

		MoonCondition* ShouldWander = new MoonCondition();
		ShouldWander->checkIfCloser = false;
		ShouldWander->earthCharacter = earth;
		ShouldWander->moonCharacter = moon;
		ShouldWander->transitionDistance = 2.0f;

		WanderingToFollowing->condition = ShouldFollow;
		FollowingToWandering->condition = ShouldWander;

		wanderState->firstTransition = WanderingToFollowing;
		followState->firstTransition = FollowingToWandering;

		moonStateMachine.initialState = wanderState;
		moonStateMachine.currentState = wanderState;







		angle = 0.0f;

		Graphics::setRenderState(DepthTest, true);
		Graphics::setRenderState(DepthTestCompare, ZCompareLess);

		Graphics::setTextureAddressing(tex, Kore::U, Repeat);
		Graphics::setTextureAddressing(tex, Kore::V, Repeat);

	}

}

	int kore(int argc, char** argv) {
		Application* app = new Application(argc, argv, width, height, false, "Exercise14");

		init();

		app->setCallback(update);

		startTime = System::time();
		Kore::Mixer::init();
		Kore::Audio::init();
		//Kore::Mixer::play(new SoundStream("back.ogg", true));

		Keyboard::the()->KeyDown = keyDown;
		Keyboard::the()->KeyUp = keyUp;
		Mouse::the()->Move = mouseMove;
		Mouse::the()->Press = mousePress;
		Mouse::the()->Release = mouseRelease;

		app->start();

		delete app;

		return 0;
	}

