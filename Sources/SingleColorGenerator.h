#pragma once

#include "TextureGenerator.h"


// Generate a texture filled with only a single color
class SingleColorGenerator: public TextureGenerator
{
	float r;
	float g;
	float b;
	float a;


public:
	
	
	virtual Texture* Generate(int width, int height);

	SingleColorGenerator(float r, float g, float b, float a);
	virtual ~SingleColorGenerator(void);
};
