#pragma once

typedef struct material material;
typedef struct hit_record
{
	float3 p;
	float3 normal;
	material *mat_ptr;
	float t;
	float2 uv;
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
	bool(*bounding_box)(struct hittable const *this, float t0, float t1, aabb *output_box);
};

/* an abstract class */
typedef struct hittable
{
	struct hittable_vtable *vtable;	
} hittable;

typedef struct translate
{
    hittable parent;
    hittable *ptr;
    float3 offset;
} translate;

internal bool translate_hit(translate const *this, ray const *r, float t_min, float t_max, hit_record *rec)
{
    ray moved_r = {r->pos - this->offset, r->dir, r->time};
    if(!this->ptr->vtable->hit(this->ptr, &moved_r, t_min, t_max, rec))
        return false;

    rec->p += this->offset;
    hit_record_set_face_normal(rec, &moved_r, rec->normal);

    return true;
}

internal bool translate_bounding_box(translate const *this, float t0, float t1, aabb *output_box)
{
    if(!this->ptr->vtable->bounding_box(this->ptr, t0, t1, output_box))
        return false;
    *output_box = (aabb){output_box->min + this->offset,
                         output_box->max + this->offset};
    return true;
}

internal translate make_translate(hittable *ptr, float3 displacement)
{
    translate result = {
		.ptr = ptr,
		.offset = displacement
	};
    /* setup vtable */
    static struct hittable_vtable translate_vtable = {
        .hit = (void*)&translate_hit,
        .bounding_box = (void*)&translate_bounding_box
    };
    result.parent.vtable = &translate_vtable;

    return result;
}

typedef struct rotate_y
{
    hittable parent;
    hittable *ptr;
    aabb bbox;
    float sin_theta;
    float cos_theta;
    bool hasbox;
} rotate_y;

internal bool rotate_y_bounding_box(rotate_y const *this, float t0, float t1, aabb *output_box)
{
	*output_box = this->bbox;
	return this->hasbox;
}

internal bool rotate_y_hit(rotate_y const *this, ray const *r, float t_min, float t_max, hit_record *rec)
{
	float3 origin = r->pos;
	float3 direction = r->dir;

	origin[0] = this->cos_theta * r->pos[0] - this->sin_theta * r->pos[2];
	origin[2] = this->sin_theta * r->pos[0] + this->cos_theta * r->pos[2];

	direction[0] = this->cos_theta * r->dir[0] - this->sin_theta * r->dir[2];
	direction[2] = this->sin_theta * r->dir[0] + this->cos_theta * r->dir[2];

	ray rotated_ray = {origin, direction, r->time};

	if(!this->ptr->vtable->hit(this->ptr, &rotated_ray, t_min, t_max, rec))
		return false;
		
	float3 p = rec->p;
	float3 normal = rec->normal;

	p[0] =  this->cos_theta * rec->p[0] + this->sin_theta * rec->p[2];
	p[2] = -this->sin_theta * rec->p[0] + this->cos_theta * rec->p[2];


	normal[0] =  this->cos_theta * rec->normal[0] + this->sin_theta * rec->normal[2];
	normal[2] = -this->sin_theta * rec->normal[0] + this->cos_theta * rec->normal[2];

	rec->p = p;
	rec->normal = normal;

	return true;
}

internal rotate_y make_rotate_y(hittable *p, float angle)
{
	float radians = degrees_to_radians(angle);
	rotate_y result = {
		.ptr = p,
		.sin_theta = sinf(radians),
		.cos_theta = cosf(radians),
		.hasbox = p->vtable->bounding_box(p, 0, 1, &result.bbox)
	};

	float3 min = INFINITY;
	float3 max = -INFINITY;

	for(i32 i = 0; i < 2; ++i)
		for(i32 j = 0; j < 2; ++j)
			for(i32 k = 0; k < 2; ++k)
			{
				float x = i * result.bbox.max.x + (1 - i) * result.bbox.min.x;
				float y = i * result.bbox.max.y + (1 - i) * result.bbox.min.y;
				float z = i * result.bbox.max.z + (1 - i) * result.bbox.min.z;

				float newx =  result.cos_theta * x + result.sin_theta * z;
				float newz = -result.sin_theta * x + result.cos_theta * z;

				float3 tester = {newx, y, newz};

				for(i32 c = 0; c < 3; ++c)
				{
					min[c] = fminf(min[c], tester[c]);
					max[c] = fmaxf(max[c], tester[c]);
				}
			}

	result.bbox = (aabb){min, max};


	/* setup vtable */
	static struct hittable_vtable rotate_y_vtable = {
		.hit = (void*)&rotate_y_hit,
		.bounding_box = (void*)&rotate_y_bounding_box
	};
	result.parent.vtable = &rotate_y_vtable;
	
	return result;
}