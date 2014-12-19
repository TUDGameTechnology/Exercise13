#pragma once
#include "TextureGenerator.h"


// Computes perlin noise with a specified frequency and amplitude
class PerlinNoiseGenerator :
	public TextureGenerator
{
	// The random gradients
	static vec2 gradients[8];
	
	// The number of gradients
	static const int numGradients = 8;

	// The permutation table (size 256)
	static int perm[];

	// Calculate perlin noise in 2D
	float perlinNoise(float x, float y);

	// Frequency (how many integer positions we are sampling)
	float frequency;

	// Amplitude: How much the noise contributes
	float amplitude;

public:
	virtual Texture* Generate(int width, int height);

	PerlinNoiseGenerator(float frequency, float amplitude);
};

