#include "pch.h"

#include <Kore/Application.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/Math/Random.h>
#include <Kore/System.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/KeyEvent.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Audio/Mixer.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>
#include <Kore/Log.h>
#include "ObjLoader.h"

#include "Collision.h"
#include "PhysicsWorld.h"
#include "PhysicsObject.h"

using namespace Kore;




namespace {
	const int width = 1024;
	const int height = 768;
	double startTime;
	Shader* vertexShader;
	Shader* fragmentShader;
	Program* program;

	float angle = 0.0f;


	bool left;
	bool right;
	bool up;
	bool down;

	// null terminated array of MeshObject pointers
	MeshObject* objects[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	// The sound to play for the winning condition
	Sound* winSound;


	// The view projection matrix aka the camera
	mat4 P;
	mat4 View;
	mat4 PV;

	vec3 cameraPosition;

	MeshObject* sphere;
	PhysicsObject* po;

	PhysicsWorld physics;
	

	// uniform locations - add more as you see fit
	TextureUnit tex;
	ConstantLocation pvLocation;
	ConstantLocation mLocation;

	double lastTime;

	void update() {
		double t = System::time() - startTime;
		double deltaT = t - lastTime;
		//Kore::log(Info, "%f\n", deltaT);
		lastTime = t;
		Kore::Audio::update();
		
		Graphics::begin();
		Graphics::clear(Graphics::ClearColorFlag | Graphics::ClearDepthFlag, 0xff9999FF, 1000.0f);

		program->set();

		

		/*
		Set your uniforms for the light vector, the roughness and all further constants you encounter in the BRDF terms.
		The BRDF itself should be implemented in the fragment shader.
		*/

		// set the camera

		angle += 0.3f * deltaT;

		float x = 0 + 10 * Kore::cos(angle);
		float z = 0 + 10 * Kore::sin(angle);
		
		cameraPosition.set(x, 2, z);

		
		cameraPosition = physics.physicsObjects[0]->GetPosition();
		cameraPosition = cameraPosition + vec3(-10, 5, 10);
		vec3 lookAt = physics.physicsObjects[0]->GetPosition();
		

		// Follow the ball with the camera
		P = mat4::Perspective(60, (float)width / (float)height, 0.1f, 100);
		View = mat4::lookAt(cameraPosition, lookAt, vec3(0, 1, 0)); 
		PV = P * View;


		Graphics::setMatrix(pvLocation, PV);





		// iterate the MeshObjects
		MeshObject** current = &objects[0];
		while (*current != nullptr) {
			// set the model matrix
			Graphics::setMatrix(mLocation, (*current)->M);

			(*current)->render(tex);
			++current;
		} 

		

		physics.Update(deltaT);


		PhysicsObject** currentP = &physics.physicsObjects[0];
	

		// Handle mouse inputs
		float forceX = 0.0f;
		float forceZ = 0.0f;
		if (up) forceX += 1.0f;
		if (down) forceX -= 1.0f;
		if (left) forceZ -= 1.0f;
		if (right) forceZ += 1.0f;

		vec3 force(forceX, 0.0f, forceZ);
		force = force * 20.0f;
		(*currentP)->ApplyForceToCenter(force);



		while (*currentP != nullptr) {
			(*currentP)->UpdateMatrix();
			Graphics::setMatrix(mLocation, (*currentP)->Mesh->M);
			(*currentP)->Mesh->render(tex);
			++currentP;
		}
		


		Graphics::end();
		Graphics::swapBuffers();
	}

	void SpawnSphere(vec3 Position, vec3 Velocity) {
		PhysicsObject* po = new PhysicsObject();
		po->SetPosition(Position);
		po->Velocity = Velocity;
		po->Collider.radius = 0.5f;

		po->Mass = 5;
		po->Mesh = sphere;
			
		// The impulse should carry the object forward
		// Use the inverse of the view matrix

		po->ApplyImpulse(Velocity);
		physics.AddObject(po);
	}

	void keyDown(KeyEvent* event) {
		if (event->keycode() == Key_Space) {
		} else if (event->keycode() == Key_Up) {
			up = true;
		} else if (event->keycode() == Key_Down) {
			down = true;
		} else if (event->keycode() == Key_Left) {
			right = true;
		} else if (event->keycode() == Key_Right) {
			left = true;
		}
	}

	void keyUp(KeyEvent* event) {
		if (event->keycode() == Key_Up) {
			up = false;
		} else if (event->keycode() == Key_Down) {
			down = false;
		} else if (event->keycode() == Key_Left) {
			right = false;
		} else if (event->keycode() == Key_Right) {
			left = false;
		}
	}

	void mouseMove(int x, int y) {

	}
	
	void mousePress(int button, int x, int y) {

	}

	

	void mouseRelease(int button, int x, int y) {
		
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
		pvLocation = program->getConstantLocation("PV");
		mLocation = program->getConstantLocation("M");

		objects[0] = new MeshObject("Base.obj", "Level/basicTiles6x6.png", structure);
		objects[0]->M = mat4::Translation(0.0f, -100.0f, 0.0f);

		objects[1] = new MeshObject("Level/Level_top.obj", "Level/basicTiles6x6.png", structure);
		objects[2] = new MeshObject("Level/Level_yellow.obj", "Level/basicTiles3x3yellow.png", structure);
		objects[3] = new MeshObject("Level/Level_red.obj", "Level/basicTiles3x3red.png", structure);

		sphere = new MeshObject("ball_at_origin.obj", "Level/unshaded.png", structure);

		SpawnSphere(vec3(-30, 1.8f, 30.f), vec3(0, 0, 0));

		physics.meshCollider.mesh = objects[1];
		
		// Sound source: http://opengameart.org/content/level-up-sound-effects
		// Task 1.3: Play this sound when the goal area is reached
		winSound = new Sound("chipquest.wav");
		Mixer::play(winSound);

		Graphics::setRenderState(DepthTest, true);
		Graphics::setRenderState(DepthTestCompare, ZCompareLess);

		Graphics::setTextureAddressing(tex, U, Repeat);
		Graphics::setTextureAddressing(tex, V, Repeat);

		

		

	}
}

int kore(int argc, char** argv) {
	Application* app = new Application(argc, argv, width, height, false, "Exercise8");
	Kore::Mixer::init();
	Kore::Audio::init();

	init();

	app->setCallback(update);

	startTime = System::time();
	lastTime = 0.0f;
	
	
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Move = mouseMove;
	Mouse::the()->Press = mousePress;
	Mouse::the()->Release = mouseRelease;

	app->start();

	delete app;
	
	return 0;
}
