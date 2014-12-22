#include "PerlinNoiseGenerator.h"
#include "Kore/Math/Random.h"


// 8 Gradients on the unit circle
vec2 PerlinNoiseGenerator::gradients[8];

// The permutation of the first 256 integers
int PerlinNoiseGenerator::perm[256] = {151,160,137,91,90,15,
 131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
 190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
 88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
 77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
 102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
 135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
 5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
 223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
 129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
 251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
 49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
 138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};


// Calculates the combination function for Perlin noise
 float fade(float t) {
	return t*t*t*(t*(t*6-15)+10);
 }

 // Interpolate two values with the specified interpolation value t
 float mix(float a, float b, float t) {
	return (1-t)*a + t*b;
 }

 PerlinNoiseGenerator::PerlinNoiseGenerator(float frequency, float amplitude) {
	 this->frequency = frequency;
	 this->amplitude = amplitude;
 }


float PerlinNoiseGenerator::perlinNoise(float x, float y) {
	// Find unit grid cell containing the point
	
	// Get relative xy coordinates of point within that cell

	// Wrap the integer cells at 255 (smaller integer period can be introduced here) - Modulo operation
	 
	// Calculate a set of four gradient indices using the permutations
	
	// Calculate noise contributions from each of the four corners using the dot product
	
	// Compute the fade curve value for x and y
	
	// Interpolate along x the contributions 
		
	 // Interpolate the two last results along y
	return 0.0f;
}

Texture* PerlinNoiseGenerator::Generate(int width, int height) {
	// Generate the gradients by choosing numGradients random 2D vectors and normalizing them
	for (int i = 0; i < numGradients; i++) {
		s32 r = Random::get(0, 99999);
		float r1 = r / 99999.0f;
		r = Random::get(0, 99999);
		float r2 = r / 99999.0f;
		vec2 gradient(r1 - 0.5f, r2-0.5f);
		gradient.normalize();
		gradients[i] = gradient;
	}

	
	Texture* texture = new Texture(width, height, Kore::Texture::RGBA32, true);
	u8* data = texture->lock();


	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			// Calculate perlin noise for the pixel
			float perlin = perlinNoise((float)x/(float)width * frequency, (float)y/(float)height * frequency) * amplitude;

			u8* values = &data[x * 4 + y * width * 4]; 		
			
			// Convert the value to grayscale
			values[0] = values[1] = values[2] = (perlin) / 2.0f * 255.0f;
			values[3] = 255;
		}
	}

	
	
	texture->unlock();


	return texture;
}

