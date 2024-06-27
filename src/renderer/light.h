#pragma once
#include "template/tmpl8math.h"

struct PointLight
{
    float3 position = float3(0);
    float3 color = float3(0.9f, 0.2f, 0.8f);
    float radius = 1.0f;
};

struct SpotLight
{
    float3 position = float3(0);
    float3 color = float3(0.5f, 1.0f, 0.0f);
    float3 direction = float3(0);
    float radius = 1.0f;
    float cutoff = 0.2f;
    float outerCutoff = 0.8f;
};

struct DirectionalLight
{
    float3 direction = float3(0);
    float3 color = float3(1.0f, 1.0f, 1.0f);
};

struct AreaLight
{
public:
    float3 position = float3(0, 2, 0);
    float3 direction = float3(0, -1, 0);
    float3 color = float3(0.15f, 0.5f, 1.0f);
    float width = 0.1f;
    float height = 0.1f;
};