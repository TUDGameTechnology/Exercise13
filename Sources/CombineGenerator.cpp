#include "CombineGenerator.h"

Texture* CombineGenerator::Generate(int width, int height) {
	Texture* texture = new Texture(width, height, Kore::Texture::RGBA32, true);
	texture1 = inputs[0]->Generate(width, height);
	texture2 = inputs[1]->Generate(width, height);

	u8* data = texture->lock();

	u8* data1 = texture1->lock();
	u8* data2 = texture2->lock();

	

	for (int i = 0; i < width*height*4; i += 4) {
		// TODO: For now, only an overlay mode
		u8* values = &data[i];
		u8* values1 = &data1[i];		
		u8* values2 = &data2[i];

		float alpha = values2[3] / 255.0f;
		for (int j = 0; j < 3; j++) {
			values[j] = values1[j] * (1 - alpha) + values2[j] * alpha;
		}
		values[3] = 255;

	}
	
	
	texture1->unlock();
	texture2->unlock();
	texture->unlock();
	


	return texture;
}


CombineGenerator::CombineGenerator()
{


}


CombineGenerator::~CombineGenerator(void)
{
}
