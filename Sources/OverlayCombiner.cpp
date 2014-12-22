#include "OverlayCombiner.h"


Texture* OverlayCombiner::Generate(int width, int height) {
	Texture* texture = new Texture(width, height, Kore::Texture::RGBA32, true);
	u8* values;
	u8* valuesBottom;
	u8* valuesTop;
	u8* data = texture->lock();


	TextureGenerator* bottomGenerator = inputs[0];
	TextureGenerator* topGenerator = inputs[1];

	Texture* bottom = bottomGenerator->Generate(width, height);
	Texture* top = topGenerator->Generate(width, height);

	u8* dataBottom = bottom->lock();
	u8* dataTop = top->lock();

	

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			// Task 1.3: Implement the overlay operation here
		}
	}


	
	texture->unlock();
	bottom->unlock();
	top->unlock();


	return texture;
}

