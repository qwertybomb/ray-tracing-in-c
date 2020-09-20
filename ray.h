#pragma once
typedef struct ray
{
	float3 pos;
	float3 dir;
	float time;
} ray;

internal inline float3 ray_at(ray const *this, float t)
{
	return this->pos + t * this->dir;
}
