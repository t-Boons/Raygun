#include "precomp.h"
#include "stb_image.h"

Skybox::Skybox(const char* filePath)
{
	stbi_set_flip_vertically_on_load(1);

	texture = stbi_loadf(filePath, &width, &height, &channels, 0);

	rotation = 0;
	pixelSize.x = 1.0f / width;
	pixelSize.y = 1.0f / height;

	assert(texture);
}