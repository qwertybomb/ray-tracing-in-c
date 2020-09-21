#pragma once
typedef struct constant_medium
{
    hittable parent;
    hittable *boundary;
    material *phase_function;
    float neg_inv_density;
} constant_medium;

internal bool constant_medium_bounding_box(constant_medium const *this, float t0, float t1, aabb *output_box)
{
    return this->boundary->vtable->bounding_box(this->boundary, t0, t1, output_box);
}

internal bool constant_medium_hit(constant_medium const *this, ray const *r, float t_min, float t_max, hit_record *rec)
{
    hit_record rec1, rec2;

    if(!this->boundary->vtable->hit(this->boundary, r, -INFINITY, INFINITY, &rec1))
        return false;

    if(!this->boundary->vtable->hit(this->boundary, r, rec1.t + 0.0001f, INFINITY, &rec2))
        return false;

    if(rec1.t < t_min) rec1.t = t_min;
    if(rec2.t > t_max) rec2.t = t_max;

    if(rec1.t >= rec2.t)
        return false;

    if(rec1.t < 0)
        rec1.t = 0;

    float ray_length = length(r->dir);
    float distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    float hit_distance = this->neg_inv_density * logf(randomf());

    if(hit_distance > distance_inside_boundary)
        return false;

    rec->t = rec1.t + hit_distance /  ray_length;
    rec->p = ray_at(r, rec->t);
    rec->normal = (float3){1, 0, 0};
    rec->front_face = false;
    rec->mat_ptr = this->phase_function;

    return true;
}

__attribute__((overloadable)) internal constant_medium make_constant_medium(hittable *bounds, float density, texture *a)
{
    constant_medium result = {
        .boundary = bounds,
        .neg_inv_density = -1 / density,
        .phase_function = new_isotropic(a)
    };

    /* setup vtable */
    static struct hittable_vtable constant_medium_vtable = {
        .hit = (void*)&constant_medium_hit,
        .bounding_box = (void*)&constant_medium_bounding_box
    };
    result.parent.vtable = &constant_medium_vtable;

    return result;
}

__attribute__((overloadable)) internal constant_medium make_constant_medium(hittable *bounds, float density, float3 color)
{
    return make_constant_medium(bounds, density, new_solid_color(color));
}
