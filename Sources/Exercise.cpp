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
#include "ObjLoader.h"
#include "SingleColorGenerator.h"
#include "GridGenerator.h"
#include "CombineGenerator.h"
#include "PerlinNoiseGenerator.h"
#include "GaussianBlurFilter.h"
#include "OverlayCombiner.h"


using namespace Kore;

class MeshObject {
public:
	MeshObject(const char* meshFile, const char* textureFile, const VertexStructure& structure, float scale = 1.0f) {
		mesh = loadObj(meshFile);
		image = new Texture(textureFile, true);

		vertexBuffer = new VertexBuffer(mesh->numVertices, structure);
		float* vertices = vertexBuffer->lock();
		for (int i = 0; i < mesh->numVertices; ++i) {
			vertices[i * 8 + 0] = mesh->vertices[i * 8 + 0] * scale;
			vertices[i * 8 + 1] = mesh->vertices[i * 8 + 1] * scale;
			vertices[i * 8 + 2] = mesh->vertices[i * 8 + 2] * scale;
			vertices[i * 8 + 3] = mesh->vertices[i * 8 + 3];
			vertices[i * 8 + 4] = 1.0f - mesh->vertices[i * 8 + 4];
			vertices[i * 8 + 5] = mesh->vertices[i * 8 + 5];
			vertices[i * 8 + 6] = mesh->vertices[i * 8 + 6];
			vertices[i * 8 + 7] = mesh->vertices[i * 8 + 7];
		}
		vertexBuffer->unlock();

		indexBuffer = new IndexBuffer(mesh->numFaces * 3);
		int* indices = indexBuffer->lock();
		for (int i = 0; i < mesh->numFaces * 3; i++) {
			indices[i] = mesh->indices[i];
		}
		indexBuffer->unlock();

		M = mat4::Identity();
	}

	void render(TextureUnit tex) {
		image->set(tex);
		vertexBuffer->set();
		indexBuffer->set();
		Graphics::drawIndexedVertices();
	}

	void setTexture(Texture* tex) {
		image = tex;
	}

	Texture* getTexture() {
		return image;
	}

	mat4 M;
private:
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	Mesh* mesh;
	Texture* image;
};

namespace {
	const int width = 1024;
	const int height = 768;
	double startTime;
	Shader* vertexShader;
	Shader* fragmentShader;
	Program* program;

	// null terminated array of MeshObject pointers
	MeshObject* objects[] = { nullptr, nullptr, nullptr, nullptr, nullptr };

	// The view projection matrix aka the camera
	mat4 P;
	mat4 V;

	// uniform locations - add more as you see fit
	TextureUnit tex;
	ConstantLocation pLocation;
	ConstantLocation vLocation;
	ConstantLocation mLocation;

	float angle;

	void update() {
		float t = (float)(System::time() - startTime);
		Kore::Audio::update();
		
		Graphics::begin();
		Graphics::clear(Graphics::ClearColorFlag | Graphics::ClearDepthFlag, 0xff9999FF, 1000.0f);
		
		program->set();

	
		// set the camera
		P = mat4::Perspective(60, (float)width / (float)height, 0.1f, 100);
		V = mat4::lookAt(vec3(0, 0, -4), vec3(0, 0, 0), vec3(0, 1, 0));
		Graphics::setMatrix(pLocation, P);
		Graphics::setMatrix(vLocation, V);


		angle = t;


		objects[0]->M = mat4::RotationY(angle) * mat4::RotationZ(Kore::pi / 4.0f);


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

	// 0: Basic example: Red grid on white ground
	// 1: Perlin Noise
	// 2: Blur Filter
	// 3: Overlay
	// 4: Your own network
	int currentTexture = -1;

	SingleColorGenerator* redGen;
	SingleColorGenerator* whiteGen;
	CombineGenerator* combineGen;
	GaussianBlurFilter* gaussianBlur;
	GridGenerator* gridGen;
	OverlayCombiner* overlay;
	PerlinNoiseGenerator* perlin;
	ImageGenerator* imageGen;

	

	void SwitchTexture() {
		currentTexture++;
		if (currentTexture == 5) {
			currentTexture = 0;
		}
		
		TextureGenerator* endNode = nullptr;

		switch (currentTexture) {
		case 0:
			endNode = combineGen;
			combineGen->ClearInputs();
			combineGen->AddInput(whiteGen);
			combineGen->AddInput(gridGen);
			break;
		
		case 1:
			endNode = perlin;
			break;

		case 2:
			endNode = gaussianBlur;
			gaussianBlur->ClearInputs();
			gaussianBlur->AddInput(imageGen);
			break;

		case 3:
			endNode = overlay;
			overlay->ClearInputs();
			overlay->AddInput(imageGen);
			overlay->AddInput(redGen);
			break;

		

		case 4:
			endNode = whiteGen;
			// Task 1.4: Implement your own effect here

			break;
		}

		objects[0]->setTexture(endNode->Generate(512, 512));

	}

	void keyDown(KeyEvent* event) {
		if (event->keycode() == Key_Space) {
			SwitchTexture();
		}
	}

	void keyUp(KeyEvent* event) {
		if (event->keycode() == Key_Left) {
			// ...
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

		objects[0] = new MeshObject("box.obj", "darmstadt.jpg", structure);

		// Solid red
		redGen = new SingleColorGenerator(0, 0, 1, 1);

		// Solid white
		whiteGen = new SingleColorGenerator(1, 1, 1, 1);

		// Combine
		combineGen = new CombineGenerator();

		// Gaussian blur
		gaussianBlur = new GaussianBlurFilter(10, 2.0f);

		// Grid
		gridGen = new GridGenerator(8, 24, 1, 0, 0, 1);

		// Overlay
		overlay = new OverlayCombiner();

		// Perlin noise
		perlin = new PerlinNoiseGenerator(20, 1);

		// Image
		imageGen = new ImageGenerator(objects[0]->getTexture());



		SwitchTexture();


		angle = 0.0f;

		Graphics::setRenderState(DepthTest, true);
		Graphics::setRenderState(DepthTestCompare, ZCompareLess);

		Graphics::setTextureAddressing(tex, Kore::U, Repeat);
		Graphics::setTextureAddressing(tex, Kore::V, Repeat);
	}
}

int kore(int argc, char** argv) {
	Application* app = new Application(argc, argv, width, height, false, "Exercise10");
	
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
