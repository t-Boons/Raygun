#pragma once
#include "CL/cl.hpp"


class Denoiser
{
public:
	void Init(const int width, const int height);

	void Filter(const float4* input, float4* output);

	void SetComparer(const float4* comparer);

public:
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;
	cl_mem inputImage, outputImage, filterImage;
	int imageBufferSize;
	int width;
	int height;
};