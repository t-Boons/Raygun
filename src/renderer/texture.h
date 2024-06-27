#pragma once

class Texture
{
public:
	Texture(const std::string& filePath);

	float3 GetPixel(float u, float v, int mipmap);
	int NumMipMaps() const { return (int)mipmapResolutions.size(); }

	int GetWidth() const { return width; }

	const std::string& FilePath() const { return filePath; }

private:
	std::vector<float*> textureBuffers;
	std::vector<int> mipmapResolutions;

	int width, height, channels;
	std::string filePath;
};