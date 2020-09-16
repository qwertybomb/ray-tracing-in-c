#pragma once
#define PI (3.141592653589f)

float random_float(void);

internal inline void write_pixel(u32 *pixel, float3 color, i32 sample_count)
{
	/* average the samples and do gamma correction */
	{
		color /= sample_count;
		color[0] = sqrt(color[0]);
		color[1] = sqrt(color[1]);
		color[2] = sqrt(color[2]);
	}
	*pixel = ((i32)(color[0] * 255.999f) << 24) | ((i32)(color[1] * 255.999f) << 16) | ((i32)(color[2] * 255.999f) << 8); 
}

internal inline size_t index_at(size_t x, size_t y, size_t width)
{
	return y  * width + x; 
}

internal inline float degrees_to_radians(float degrees) 
{
    return degrees * PI / 180.0f;
}

internal inline float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

/* random number genreration */
__attribute__((overloadable)) internal inline float randomf(void)
{
	return random_float();
}

__attribute__((overloadable)) internal inline float randomf(float min, float max)
{
	return min + (max - min) * randomf();
}

__attribute__((overloadable)) internal inline float2 randomf2(void)
{
	return (float2){randomf(), randomf()};
}

__attribute__((overloadable)) internal inline float3 randomf3(void)
{
	return (float3){randomf(), randomf(), randomf()};
}

__attribute__((overloadable)) internal inline float4 randomf4(void)
{
	return (float4){randomf(), randomf(), randomf(), randomf()};
}

__attribute__((overloadable)) internal inline float2 randomf2(float min, float max)
{
	return (float2){randomf(min, max), randomf(min, max)};
}

__attribute__((overloadable)) internal inline float3 randomf3(float min, float max)
{
	return (float3){randomf(min, max), randomf(min, max), randomf(min, max)};
}

__attribute__((overloadable)) internal inline float4 randomf4(float min, float max)
{
	return (float4){randomf(min, max), randomf(min, max), randomf(min, max), randomf(min, max)};
}

internal inline float3 random_in_unit_sphere(void)
{
	for(;;)
	{
		float3 p = randomf3(-1, 1);
		if(length_sqr(p) >= 1) continue;
		return p;
	}
}

internal inline float3 random_unit_vector(void) {
    float a = randomf(0, 2 * PI);
    float z = randomf(-1, 1);
    float r = sqrtf(1 - z*z);
    return (float3){r * cosf(a), r * sinf(a), z};
}

internal inline float3 random_in_unit_disk(void)
{
	for(;;)
	{
		float3 p = {randomf(-1, 1), randomf(-1, 1), 0};
		if(length_sqr(p) >= 1) continue;
		return p;
	}
}
