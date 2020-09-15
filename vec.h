#pragma once
#include <math.h>

/* types */
typedef float float2 __attribute__((ext_vector_type(2)));
typedef float float3 __attribute__((ext_vector_type(3)));
typedef float float4 __attribute__((ext_vector_type(4)));

/* length squared */
__attribute__((overloadable)) internal inline float length_sqr(float2 v)
{
	return v.x * v.x + v.y * v.y; 
}

__attribute__((overloadable)) internal inline float length_sqr(float3 v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z; 
}

__attribute__((overloadable)) internal inline float length_sqr(float4 v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w; 
}

/* length */
__attribute__((overloadable)) internal inline float length(float2 v)
{
	return sqrtf(length_sqr(v)); 
}

__attribute__((overloadable)) internal inline float length(float3 v)
{
	return sqrtf(length_sqr(v)); 
}

__attribute__((overloadable)) internal inline float length(float4 v)
{
	return sqrtf(length_sqr(v)); 
}

/* dot product */
__attribute__((overloadable)) internal inline float dot(float2 u, float2 v)
{
	return u[0] * v[0] + u[1] * v[1]; 
}

__attribute__((overloadable)) internal inline float dot(float3 u, float3 v)
{
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2]; 
}

__attribute__((overloadable)) internal inline float dot(float4 u, float4 v)
{
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2] + v[3] * v[3]; 
}

/* cross product */
__attribute__((overloadable)) internal inline float3 cross(float3 u, float3 v)
{
	return (float3){u[1] * v[2] - u[2] * v[1],
					u[2] * v[0] - u[0] * v[2],
					u[0] * v[1] - u[1] * v[0]};
}

/* normalization */
__attribute__((overloadable)) internal inline float2 normalize(float2 v)
{
	return v / length(v);
}

__attribute__((overloadable)) internal inline float3 normalize(float3 v)
{
	return v / length(v);
}

__attribute__((overloadable)) internal inline float4 normalize(float4 v)
{
	return v / length(v);
}

/* reflection */
internal inline float3 reflect(float3 v, float3 n)
{
	return v - 2 * dot(v, n) * n;
}

/* refraction */
internal inline float3 refract(float3 uv, float3 n, float etai_over_etat)
{
	float cos_theta = dot(-uv, n);
	float3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	float3 r_out_parallel = -sqrtf(fabsf(1.0f - length_sqr(r_out_perp))) * n;
	return r_out_parallel + r_out_perp;
}
