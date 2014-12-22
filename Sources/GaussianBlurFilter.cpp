#include "GaussianBlurFilter.h"

// Calculate the gaussian with mu = 0 and the specified sigma
float Gaussian(float x, float y, float sigma) {
	// Task 1.2: Calculate the value of the Gaussian	
	return 0.5f;
}


GaussianBlurFilter::GaussianBlurFilter(int kernelSize, float sigma) {
	this->kernelSize = kernelSize;
	this->sigma = sigma;
}


void GaussianBlurFilter::ApplyKernel(u8* original, u8* pixel, int x, int y, float* kernel, int width, int height) {
	float values[4];
	values[0] = values[1] = values[2] = values[3] = 0.0f;
	
	// Task 1.2: Apply the kernel here - Treat the border pixels by extending them beyond the image borders

	// Set the pixel
	for (int i = 0; i < 3; i++) {
		pixel[i] = values[i];
	}
	pixel[3] = 255;
}


Texture* GaussianBlurFilter::Generate(int width, int height) {
	Texture* texture = new Texture(width, height, Kore::Texture::RGBA32, true);
	u8 values[4];
	u8* data = texture->lock();
	
	Texture* input = inputs[0]->Generate(width, height);
	
	u8* inputData = input->lock();

	

	float* kernel = new float[kernelSize*kernelSize];
	int kernelHalfSize = (int) kernelSize / 2;
	for (int x = 0; x < kernelSize; x++) {
		for (int y = 0; y < kernelSize; y++) {
			kernel[x + y * kernelSize] = Gaussian((float) x - kernelHalfSize, (float) y - kernelHalfSize, sigma);
		}
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			ApplyKernel(inputData, &data[x * 4 + y * 4 * width], x, y, kernel, width, height);
		}
	}


	
	input->unlock();
	texture->unlock();

	return texture;
}

