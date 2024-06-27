#include "precomp.h"
#include "denoiser.h"

void Denoiser::Init(const int w, const int h)
{
	width = w; height = h;
	imageBufferSize = sizeof(float4) * width * height;


	// Load OpenCL platform
	clGetPlatformIDs(1, &platform, NULL);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

	// Create OpenCL context
	context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);

	// Create command queue
	queue = clCreateCommandQueue(context, device, 0, NULL);


	const std::string source_code = R"(

	// Code partially generated by chatgpt and modified to fit my needs. (Proud of this code c:)
	
	inline float gaussianWeight(float intensity_diff, float sigma_color)
	{
		return exp(-(intensity_diff * intensity_diff) / (2 * sigma_color * sigma_color));
	}

	inline float spatialWeight(float spatial_diff, float sigma_space)
	{
		return exp(-(spatial_diff * spatial_diff) / (2 * sigma_space * sigma_space));
	}

	__kernel void bilateralFilterKernel(__global const float4* buffer, 
										__global const float4* comparer, 
										__global float4* output)
	{

		int i = get_global_id(0);
		int j = get_global_id(1);

		int radius = 4;

		int width = get_global_size(0);
		int height = get_global_size(1);

		float4 sum = {0.0f, 0.0f, 0.0f, 0.0f};
		float4 weightSum = {0.0f, 0.0f, 0.0f, 0.0f};
		
		for (int k = -radius; k <= radius; ++k)
		{
			for (int l = -radius; l <= radius; ++l)
			{
				int ni = i + k;
				int nj = j + l;
		
				if (ni >= 0 && ni < width && nj >= 0 && nj < height)
				{
					float4 diff = fabs(comparer[nj * width + ni] - comparer[j * width + i]);
		
					float kf = (float)k; 
					float lf = (float)l;
					float spatDiff = spatialWeight(sqrt(kf * kf + lf * lf), 0.75f);
		
					float4 weight = (float4){gaussianWeight(diff.x, 0.75f),
											gaussianWeight(diff.y, 0.75f),
											gaussianWeight(diff.z, 0.75f),
											0.0f} * spatDiff;
		
					sum += buffer[nj * width + ni] * weight;
					weightSum += weight;
				}
			}
		}
		
		output[j * width + i] = sum / weightSum;
	}
)";

	const size_t size = source_code.length();
	const char* src = source_code.c_str();
	program = clCreateProgramWithSource(context, 1, &src, &size, NULL);


	const cl_int err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if (err != CL_SUCCESS) {
		// Get the build log size
		size_t logSize;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);

		// Allocate memory for the build log
		std::vector<char> buildLog(logSize);

		// Get the build log
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, buildLog.data(), nullptr);

		// Print the build log
		std::cerr << "OpenCL program build log:" << std::endl;
		std::cerr << buildLog.data() << std::endl;

		clReleaseProgram(program);
	}


	// Create kernel
	kernel = clCreateKernel(program, "bilateralFilterKernel", NULL);

	// Create memory buffers
	inputImage = clCreateBuffer(context, CL_MEM_READ_ONLY, imageBufferSize, NULL, NULL);
	outputImage = clCreateBuffer(context, CL_MEM_WRITE_ONLY, imageBufferSize, NULL, NULL);
	filterImage = clCreateBuffer(context, CL_MEM_READ_ONLY, imageBufferSize, NULL, NULL);

	// Set kernel arguments
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputImage);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &filterImage);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &outputImage);
}

void Denoiser::Filter(const float4* input, float4* output)
{
	size_t global_work_size[2] = { (size_t)width, (size_t)height };

	// Denoise filter with normals buffer.
	clEnqueueWriteBuffer(queue, inputImage, CL_TRUE, 0, imageBufferSize, input, 0, NULL, NULL);

	clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
	clEnqueueReadBuffer(queue, outputImage, CL_TRUE, 0, imageBufferSize, output, 0, NULL, NULL);
}

void Denoiser::SetComparer(const float4* comparer)
{
	clEnqueueWriteBuffer(queue, filterImage, CL_TRUE, 0, imageBufferSize, comparer, 0, NULL, NULL);
}
