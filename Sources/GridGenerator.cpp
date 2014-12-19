#include "GridGenerator.h"


Texture* GridGenerator::Generate(int width, int height) {
	Texture* texture = new Texture(width, height, Kore::Image::Format::RGBA32, true);
	u8 values[4];
	u8* data = texture->lock();
	values[0] = b * 255;
	values[1] = g * 255;
	values[2] = r * 255;
	values[3] = a * 255;
	

	

	for (int i = 0; i < width*height*4; i++) {
		int x = i % (width * 4) / 4;
		int lineX = x % (lineWidth + spacing);
		if (lineX < lineWidth) {
			data[i] = values[i%4];
		} else {
			data[i] = 0;
		}
		int y = i / (width * 4);
		int lineY = y % (lineWidth + spacing);
		if (lineY < lineWidth) {
			data[i] = values[i%4];
		}


	}
	texture->unlock();


	return texture;
}

GridGenerator::GridGenerator(int lineWidth, int spacing, float r, float g, float b, float a): lineWidth(lineWidth), spacing(spacing), r(r), g(g), b(b), a(a)  {
	

}


GridGenerator::~GridGenerator(void)
{
}
