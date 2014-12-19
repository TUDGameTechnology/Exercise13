#pragma once
#include "TextureGenerator.h"

// Generates a simple grid texture with alpha = 0 between the grid lines
class GridGenerator :
	public TextureGenerator
{
	int lineWidth;
	int spacing;
	float r;
	float g;
	float b;
	float a;


public:
	virtual Texture* Generate(int width, int height);

	/* lineWidth is the width of the grid lines
	Spacing is the distance between lines (from the actual drawn lines)  */
	GridGenerator(int lineWidth, int spacing, float r, float g, float b, float a);
	~GridGenerator(void);
};

