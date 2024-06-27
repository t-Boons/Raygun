#pragma once

class Settings
{
public:
	inline static bool stochasticLights = true;
	inline static bool multisampled = true;
	inline static bool mipmapping = true;
	inline static bool bilinearInterpolation = true;
	inline static bool denoise = true;
	inline static float fov = 45.0f;
	inline static uint32_t maxBounces = 2;
};