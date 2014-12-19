#pragma once
#include "TextureGenerator.h"

// Gaussian blur filter is applied to inputs[0]
class GaussianBlurFilter :
	public TextureGenerator
{
	
	// The kernel size - should be an odd number >= 3
	int kernelSize;

	// The standard deviation 
	float sigma;

	// Apply the kernel to the specified pixel
	// Original points to the original image, pixel to the correct pixel in the destination image
	// kernel is the filter kernel, arranged in the same way as Kore images
	// width and height are the dimensions of the image
	void ApplyKernel(u8* original, u8* pixel, int x, int y, float* kernel, int width, int height);

public:
	virtual Texture* Generate(int width, int height);

	GaussianBlurFilter(int kernelSize, float sigma);


};

