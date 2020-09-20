#pragma once

typedef struct xy_rect
{
	hittable parent;
	material *mat_ptr;
	float x0, x1, y0, y1, k;
} xy_rect;

internal bool xy_rect_bounding_box(xy_rect const *this, float t0, float t1, aabb *output_box)
{
	*output_box = (aabb){(float3){this->x0, this->y0, this->k - 0.0001f}, (float3){this->x1, this->y1, this->k - 0.0001f}};
	return true;	
}

internal bool xy_rect_hit(xy_rect const *this, ray const *r, float t0, float t1, hit_record *rec)
{
	float t = (this->k - r->pos.z) / r->dir.z;
	if(t < t0 || t > t1)
		return false;

	float x = r->pos.x + t * r->dir.x;
	float y = r->pos.y + t * r->dir.y;
	if(x < this->x0 || x > this->x1 || y < this->y0 || y > this->y1)
		return false;

	rec->uv = (float2){(x - this->x0) / (this->x1 - this->x0),
					   (y - this->y0) / (this->y1 - this->y0)};
	rec->t = t;
	float3 outward_normal = {0, 0, 1};
	hit_record_set_face_normal(rec, r, outward_normal);
	rec->mat_ptr = this->mat_ptr;
	rec->p = ray_at(r, t);
	return true;
}

internal hittable *new_xy_rect(float x0, float x1, float y0, float y1, float k, material *mat)
{
	xy_rect *result = malloc(sizeof(xy_rect));
	result->x0 = x0;
	result->x1 = x1;
	result->y0 = y0;
	result->y1 = y1;
	result->k = k;
	result->mat_ptr = mat;

	/* setup vtable */
	static struct hittable_vtable xy_rect_vtable = {
		.hit = (void*)&xy_rect_hit,
		.bounding_box = (void*)&xy_rect_bounding_box
	};
	result->parent.vtable = &xy_rect_vtable;
	return (void*)result;
}

typedef struct xz_rect
{
    hittable parent;
    material *mat_ptr;
    float x0, x1, z0, z1, k;
} xz_rect;

internal bool xz_rect_bounding_box(xz_rect const *this, float t0, float t1, aabb *output_box)
{
    *output_box = (aabb){(float3){this->x0, this->k - 0.0001f, this->z0}, (float3){this->x1, this->k - 0.0001f, this->z1}};
    return true;
}

internal bool xz_rect_hit(xz_rect const *this, ray const *r, float t0, float t1, hit_record *rec)
{
    float t = (this->k - r->pos.y) / r->dir.y;
    if(t < t0 || t > t1)
        return false;

    float x = r->pos.x + t * r->dir.x;
    float z = r->pos.z + t * r->dir.z;
    if(x < this->x0 || x > this->x1 || z < this->z0 || z > this->z1)
        return false;

    rec->uv = (float2){(x - this->x0) / (this->x1 - this->x0),
                       (z - this->z0) / (this->z1 - this->z0)};
    rec->t = t;
    float3 outward_normal = {0, 1, 0};
    hit_record_set_face_normal(rec, r, outward_normal);
    rec->mat_ptr = this->mat_ptr;
    rec->p = ray_at(r, t);
    return true;
}

internal hittable *new_xz_rect(float x0, float x1, float z0, float z1, float k, material *mat)
{
    xz_rect *result = malloc(sizeof(xy_rect));
    result->x0 = x0;
    result->x1 = x1;
    result->z0 = z0;
    result->z1 = z1;
    result->k = k;
    result->mat_ptr = mat;

    /* setup vtable */
    static struct hittable_vtable xz_rect_vtable = {
        .hit = (void*)&xz_rect_hit,
        .bounding_box = (void*)&xz_rect_bounding_box
    };
    result->parent.vtable = &xz_rect_vtable;
    return (void*)result;
}

typedef struct yz_rect
{
    hittable parent;
    material *mat_ptr;
    float y0, y1, z0, z1, k;
} yz_rect;

internal bool yz_rect_bounding_box(yz_rect const *this, float t0, float t1, aabb *output_box)
{
    *output_box = (aabb){(float3){this->k - 0.0001f, this->y0, this->z0}, (float3){this->k - 0.0001f, this->y1, this->z1}};
    return true;
}

internal bool yz_rect_hit(yz_rect const *this, ray const *r, float t0, float t1, hit_record *rec)
{
    float t = (this->k - r->pos.x) / r->dir.x;
    if(t < t0 || t > t1)
        return false;

    float y = r->pos.y + t * r->dir.y;
    float z = r->pos.z + t * r->dir.z;
    if(y < this->y0 || y > this->y1 || z < this->z0 || z > this->z1)
        return false;

    rec->uv = (float2){(y - this->y0) / (this->y1 - this->y0),
                       (z - this->z0) / (this->z1 - this->z0)};
    rec->t = t;
    float3 outward_normal = {1, 0, 0};
    hit_record_set_face_normal(rec, r, outward_normal);
    rec->mat_ptr = this->mat_ptr;
    rec->p = ray_at(r, t);
    return true;
}

internal hittable *new_yz_rect(float y0, float y1, float z0, float z1, float k, material *mat)
{
    yz_rect *result = malloc(sizeof(yz_rect));
    result->y0 = y0;
    result->y1 = y1;
    result->z0 = z0;
    result->z1 = z1;
    result->k = k;
    result->mat_ptr = mat;

    /* setup vtable */
    static struct hittable_vtable yz_rect_vtable = {
        .hit = (void*)&yz_rect_hit,
        .bounding_box = (void*)&yz_rect_bounding_box
    };
    result->parent.vtable = &yz_rect_vtable;
    return (void*)result;
}