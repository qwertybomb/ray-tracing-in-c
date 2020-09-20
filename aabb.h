#pragma once

typedef struct aabb
{
	float3 min;
	float3 max;
} aabb;

internal bool aabb_hit(aabb const *this, ray const *r, float tmin, float tmax)
{
	#define swap(a, b) ({__typeof__(a) temp = a; a = b; b = temp;})
	for(i32 a = 0; a < 3; ++a)
	{	
		float inv_d  = 1.0f / r->dir[a];
		float t0 = (this->min[a] - r->pos[a]) * inv_d;
		float t1 = (this->max[a] - r->pos[a]) *	inv_d;
		if(inv_d < 0.0f)
			swap(t0, t1);
			
		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax; 
		if(tmax <= tmin)
			return false;	
	}
	return true;
	#undef swap
}

internal aabb surrounding_box(aabb box0, aabb box1)
{
	float3 small = {fminf(box0.min.x, box1.min.x),
					fminf(box0.min.y, box1.min.y),
					fminf(box0.min.z, box1.min.z)};

	float3 big = {fmaxf(box0.max.x, box1.max.x),
				  fmaxf(box0.max.y, box1.max.y),
				  fmaxf(box0.max.z, box1.max.z)};

	return (aabb){small, big};
}
