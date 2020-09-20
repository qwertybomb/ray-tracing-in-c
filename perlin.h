#pragma once

enum { PERLIN_POINT_COUNT = 256 };

typedef struct perlin
{
	float3 *ranvec;
	i32 *perm_x;
	i32 *perm_y;
	i32 *perm_z;
} perlin;

internal void perlin_permute(i32 *p, i32 n)
{
	for(i32 i = n - 1; i > 0; --i)
	{
		i32 target = (i32)randomf(0, i + 1);
		i32 temp = p[i];
		p[i] = p[target];
		p[target] = temp;
	}	
}

internal i32 *perlin_generate_perm(void)
{
	i32 *p = malloc(sizeof(i32) * PERLIN_POINT_COUNT);
	for(i32 i = 0; i < PERLIN_POINT_COUNT; ++i)
		p[i] = i;
		
	perlin_permute(p, PERLIN_POINT_COUNT);

	return p;
}

internal float perlin_interp(float3 c[2][2][2], float u, float v, float w)
{
    float uu = u * u * (3 - 2 * u);
    float vv = v * v * (3 - 2 * v);
    float ww = w * w * (3 - 2 * w);
    float result = 0.0f;

    for(i32 i = 0; i < 2; ++i)
        for(i32 j = 0; j < 2; ++j)
            for(i32 k = 0; k < 2; ++k)
            {
                float3 weight_v = { u - i, v - j, w - k};
                result += ((i * uu) + (1 - i) * (1 - uu))
                        * ((j * vv) + (1 - j) * (1 - vv))
                        * ((k * ww) + (1 - k) * (1 - ww))
                        * dot(c[i][j][k], weight_v);
            }
    return result;
}

internal float perlin_noise(perlin const *this, float3 p)
{
	float u = p.x - floorf(p.x);
	float v = p.y - floorf(p.y);
	float w = p.z - floorf(p.z);

	i32 i = floorf(p.x);
    i32 j = floorf(p.y);
    i32 k = floorf(p.z);
    float3 c[2][2][2];

    for(i32 di = 0; di < 2; ++di)
        for(i32 dj = 0; dj < 2; ++dj)
            for(i32 dk = 0; dk < 2; ++dk)
            {
                c[di][dj][dk] =
                    this->ranvec[
                        this->perm_x[(i + di) & 255] ^
                        this->perm_y[(j + dj) & 255] ^
                        this->perm_z[(k + dk) & 255]
                    ];
            }

    return perlin_interp(c, u, v, w);
}

internal float perlin_turb(perlin const *this, float3 p, i32 depth)
{
    float result = 0;
    float3 temp_p = p;
    float weight = 1;

    for(i32 i = 0; i < depth; ++i)
    {
        result += weight * perlin_noise(this, temp_p);
        weight *= 0.5f;
        temp_p *= 2;
    }

    return  fabsf(result);
}

internal perlin make_perlin(void)
{
	perlin result = { .ranvec = malloc(sizeof(float3) * PERLIN_POINT_COUNT) };
	for(i32 i = 0; i < PERLIN_POINT_COUNT; ++i)
	{
		result.ranvec[i] = normalize(randomf3(-1, 1));
	}
	
	result.perm_x = perlin_generate_perm();
	result.perm_y = perlin_generate_perm();
	result.perm_z = perlin_generate_perm();

	return result;
}
