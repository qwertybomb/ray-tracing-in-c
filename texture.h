#pragma once
#include "perlin.h"

struct texture;

struct texture_vtable
{
	float3(*value)(struct texture const *this, float2 uv, float3 p);
};

typedef struct texture
{
	struct texture_vtable *vtable;	
} texture;

typedef struct solid_color
{
	texture parent;
	float3 color_value;
} solid_color;

internal float3 solid_color_value(solid_color const *this, float2 uv, float3 p)
{
	return this->color_value;
}

internal texture *new_solid_color(float3 c)
{
	solid_color *result = malloc(sizeof(solid_color));
	result->color_value = c;

	/* setup vtable */
	static struct texture_vtable solid_color_vtable = { .value = (void*)solid_color_value };
	result->parent.vtable = &solid_color_vtable;

	return (void*)result;
}

typedef struct checker_texture
{
	texture parent;
	texture *odd;
	texture *even;
} checker_texture;

internal float3 checker_texture_value(checker_texture const *this, float2 uv, float3 p)
{
    float sines = sinf(10 * p.x) * sinf(10 * p.y) * sinf(10 * p.z);
    if(sines < 0)
        return this->odd->vtable->value(this->odd, uv, p);
    else
        return this->even->vtable->value(this->even, uv, p);
}

__attribute__((overloadable)) internal texture *new_checker_texture(texture *t0, texture *t1)
{
    checker_texture *result = malloc(sizeof(checker_texture));
    result->even = t0;
    result->odd = t1;

    /* setup vtable */
    static struct texture_vtable checker_texture_vtable = { .value  = (void*)&checker_texture_value };
    result->parent.vtable = &checker_texture_vtable;

    return (void*)result;
}

__attribute__((overloadable)) internal texture *new_checker_texture(float3 c1, float3 c2)
{
    return new_checker_texture(new_solid_color(c1), new_solid_color(c2));
}

typedef struct noise_texture
{
    texture parent;
	perlin noise;
	float scale;
} noise_texture;

internal float3 noise_texture_value(noise_texture const *this, float2 uv, float3 p)
{
	return 0.5f * (1.0f + sinf(this->scale * p.z + 10 * perlin_turb(&this->noise, p, 7)));
}

__attribute__((overloadable)) internal texture *new_noise_texture(void)
{
    noise_texture *result = malloc(sizeof(noise_texture));
    result->noise = make_perlin();
    result->scale = 0;

    /* setup vtable */
    static struct texture_vtable noise_texture_vtable = { .value = (void*)&noise_texture_value };
    result->parent.vtable = &noise_texture_vtable;

    return (void*)result;
}

__attribute__((overloadable)) internal texture *new_noise_texture(float sc)
{
    noise_texture *result = (void*)new_noise_texture();
    result->scale = sc;
    return (void*)result;
}