#pragma once
#include "TextureGenerator.h"

// Simple combination of the two images. inputs[1] is drawn over inputs[0]
class CombineGenerator :
	public TextureGenerator
{

	Texture* texture1;
	Texture* texture2;


public:

	virtual Texture* Generate(int width, int height);

	CombineGenerator();
	~CombineGenerator(void);
};

