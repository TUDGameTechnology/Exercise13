#include "SingleColorGenerator.h"


Texture* SingleColorGenerator::Generate(int width, int height) {
	Texture* texture = new Texture(width, height, Kore::Image::Format::RGBA32, true);
	u8 values[4];
	u8* data = texture->lock();
	values[0] = r * 255;
	values[1] = g * 255;
	values[2] = b * 255;
	values[3] = a * 255;
	for (int i = 0; i < width*height*4; i++) {
		data[i] = values[i % 4];
	}
	texture->unlock();


	return texture;
}

SingleColorGenerator::SingleColorGenerator(float r, float g, float b, float a): r(r), g(g), b(b), a(a)
{
}


SingleColorGenerator::~SingleColorGenerator(void)
{
}
