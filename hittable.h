#pragma once

typedef struct material material;
typedef struct hit_record
{
	float3 p;
	float3 normal;
	material *mat_ptr;
	float t;
	bool front_face;
} hit_record;

internal inline void hit_record_set_face_normal(hit_record *this, ray const *r, float3 outward_normal)
{
	this->front_face = dot(r->dir, outward_normal) < 0;
	this->normal = this->front_face ? outward_normal : -outward_normal;
}

struct hittable;

struct hittable_vtable
{
	bool(*hit)(struct hittable const *this, ray const *r, float t_min, float t_max, hit_record* rec);	
};

/* an abstract class */
typedef struct hittable
{
	struct hittable_vtable *vtable;	
} hittable;
