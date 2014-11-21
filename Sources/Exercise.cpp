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
	MeshObject* objects[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	TextureUnit tex;
	ConstantLocation pvLocation;
	ConstantLocation mLocation;
	mat4 PV;

	void update() {
		float t = (float)(System::time() - startTime);
		Kore::Audio::update();
		
		Graphics::begin();
		Graphics::clear(Graphics::ClearColorFlag | Graphics::ClearDepthFlag, 0xff9999FF, 1000.0f);
		
		program->set();

		PV = mat4::Perspective(60, (float)width / (float)height, 0.1f, 100) * mat4::lookAt(vec3(0, 2, -3), vec3(0, 2, 0), vec3(0, 1, 0));
		Graphics::setMatrix(pvLocation, PV);

		MeshObject** current = &objects[0];
		while (*current != nullptr) {
			Graphics::setMatrix(mLocation, (*current)->M);
			(*current)->render(tex);
			++current;
		}

		Graphics::end();
		Graphics::swapBuffers();
	}

	void keyDown(KeyEvent* event) {
		if (event->keycode() == Key_Left) {
			// ...
		}
	}

	void keyUp(KeyEvent* event) {
		if (event->keycode() == Key_Left) {
			// ...
		}
	}

	void mouseMove(int x, int y) {

	}
	
	void mousePress(int button, int x, int y) {

	}

	void mouseRelease(int button, int x, int y) {

	}

	void init() {
		

		//mesh = loadObj("Level/level.obj");
		//image = new Texture("Level/texture.jpg", true);

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

		objects[0] = new MeshObject("Level/ball.obj", "Level/unshaded.png", structure);
		objects[0]->M = mat4::Translation(0.7f, 1.2f, -0.2f);
		objects[1] = new MeshObject("Level/Level_top.obj", "Level/basicTiles6x6.png", structure);
		objects[2] = new MeshObject("Level/Level_yellow.obj", "Level/basicTiles3x3yellow.png", structure);
		objects[3] = new MeshObject("Level/Level_red.obj", "Level/basicTiles3x3red.png", structure);

		Graphics::setRenderState(DepthTest, true);
		Graphics::setRenderState(DepthTestCompare, ZCompareLess);

		Graphics::setTextureAddressing(tex, U, Repeat);
		Graphics::setTextureAddressing(tex, V, Repeat);
	}
}

int kore(int argc, char** argv) {
	Application* app = new Application(argc, argv, width, height, false, "Exercise6");
	
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
