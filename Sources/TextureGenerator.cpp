#include "TextureGenerator.h"


void TextureGenerator::AddInput(TextureGenerator* input) {
	TextureGenerator** current = &inputs[0];
	while (*current != nullptr) {
		current++;
	}
	(*current) = input;
}

void TextureGenerator::ClearInputs() {
	for (int i = 0; i < 10; i++) {
		inputs[i] = nullptr;
	}
}

TextureGenerator::TextureGenerator(void)
{
	inputs = new TextureGenerator*[10];
	for (int i = 0; i < 10; i++) {
		inputs[i] = nullptr;
	}
}


TextureGenerator::~TextureGenerator(void)
{
}
