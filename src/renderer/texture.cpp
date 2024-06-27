#include "precomp.h"
#include "texture.h"
#include <stb_image.h>

Texture::Texture(const std::string& filePath)
{
	stbi_set_flip_vertically_on_load(1);

	float* tex = stbi_loadf(filePath.c_str(), &width, &height, &channels, 0);
	assert(tex);
	this->filePath = filePath;

	textureBuffers.push_back(tex);

	int sampleIndex = 0;
	int mipmapIndex = 1;
	while (true)
	{
		int mipmapSize = width;
		for (size_t i = 0; i < mipmapIndex; i++)
		{
			mipmapSize /= 2;
		}
		if (mipmapSize < 2) break;

		mipmapResolutions.push_back(mipmapSize * 2);

		float* newMipmap = new float[mipmapSize * mipmapSize * channels + 3];

		int i = 0;
		for (int y = 0; y < mipmapSize; ++y) for (int x = 0; x < mipmapSize; ++x)
		{
			float3 pixel = sampleFrom4Pixels(textureBuffers[sampleIndex], x * 2, y * 2, mipmapSize * 2, mipmapSize * 2, channels);
			newMipmap[i    ] = pixel.x;
			newMipmap[i + 1] = pixel.y;
			newMipmap[i + 2] = pixel.z;
			i += 3;
		}
		mipmapIndex++;
		sampleIndex++;
		textureBuffers.push_back(newMipmap);
	}
}

float3 Texture::GetPixel(float u, float v, int mipmap)
{
	const int mipmapSize = mipmapResolutions[mipmap];
	const float* buff = textureBuffers[mipmap];

	if (Settings::bilinearInterpolation)
	{
		return bilinearInterp(buff, u, v, mipmapSize, mipmapSize, channels);
	}
	else
	{
		const float fmipmapSize = (float)mipmapSize;
		const float* pix = &buff[(int)(v * fmipmapSize) * mipmapSize * channels + (int)(u * fmipmapSize) * channels];
		return float3(*pix, *(pix + 1), *(pix + 2));
	}
}
