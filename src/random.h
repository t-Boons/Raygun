#pragma once


// Taken from Jacco's template.
// WangHash: calculates a high-quality seed based on an arbitrary non-zero
// integer. Use this to create your own seed based on e.g. thread index.
inline uint32_t wangHash(uint32_t s)
{
	s = (s ^ 61) ^ (s >> 16);
	s *= 9, s = s ^ (s >> 4);
	s *= 0x27d4eb2d;
	s = s ^ (s >> 15);
	return s;
}

// random number generator - Marsaglia's xor32
// This is a high-quality RNG that uses a single 32-bit seed. More info:
// https://www.researchgate.net/publication/5142825_Xorshift_RNGs

class Random
{
public:
	Random(uint32_t seedBase)
		: seed(wangHash((seedBase + 1) * 17))
	{}

	// Taken from jacco's template.
	// RandomFloat()
	// Calculate a random unsigned int and cast it to a float in the range
	// [0..1)
	float Float() { return RandomUInt() * 2.3283064365387e-10f; }
	float Rand(float range) { return Float() * range; }

	// Xor32
	uint32_t RandomUInt()
	{
		seed ^= seed << 13;
		seed ^= seed >> 17;
		seed ^= seed << 5;
		return seed;
	}

private:
	uint32_t seed;
};