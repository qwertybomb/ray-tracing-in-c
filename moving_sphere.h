#pragma once
typedef struct moving_sphere
{
	hittable parent;
	float3 center0, center1;
	float time0, time1; 
	float radius;
	material *mat_ptr;	
} moving_sphere;

internal float3 moving_sphere_center(moving_sphere const *this, float time)
{
	return this->center0 + ((time - this->time0) / (this->time1 - this->time0)) * (this->center1 - this->center0);
}

internal bool moving_sphere_hit(moving_sphere const *this, ray const *r, float t_min, float t_max, hit_record *rec)
{
	float3 oc = r->pos - moving_sphere_center(this, r->time);
	float a = length_sqr(r->dir);
	float half_b = dot(oc, r->dir);
	float c = length_sqr(oc) - this->radius * this->radius;
	float discriminant = half_b * half_b - a * c;
	
	if(discriminant > 0.0f)
	{
		float root = sqrtf(discriminant);
		float temp = (-half_b - root) /	a;
		if(temp < t_max && temp > t_min)
		{
			rec->t = temp;
			rec->p = ray_at(r, rec->t);
			float3 outward_normal = (rec->p - moving_sphere_center(this, r->time)) / this->radius;
			hit_record_set_face_normal(rec, r, outward_normal);
			rec->mat_ptr = this->mat_ptr;
			return true;
		}

		temp = (-half_b + root) / a;
		if(temp < t_max && temp > t_min)
		{
			rec->t = temp;
			rec->p = ray_at(r, rec->t);
			float3 outward_normal = (rec->p - moving_sphere_center(this, r->time)) / this->radius;
			hit_record_set_face_normal(rec, r, outward_normal);
			rec->mat_ptr = this->mat_ptr;
			return true;
		}
	}

	return false;
}

bool moving_sphere_bounding_box(moving_sphere const *this, float t0, float t1, aabb *output_box)
{
	aabb box0 = {moving_sphere_center(this, t0) - this->radius, moving_sphere_center(this, t0) + this->radius};
	aabb box1 = {moving_sphere_center(this, t1) - this->radius, moving_sphere_center(this, t1) + this->radius};
	*output_box = surrounding_box(box0, box1);
	return true;
}

hittable *new_moving_sphere(float3 center0, float3 center1, float t0, float t1, float radius, void *m)
{
	moving_sphere *result = malloc(sizeof(moving_sphere));

	/* assign the function paremeters to their respective struct members */
	result->center0 = center0;
	result->center1 = center1;
	result->time0 = t0;
	result->time1 = t1;
	result->radius = radius;
	result->mat_ptr = m;
	
	/* setup vtable */
	static struct hittable_vtable moving_sphere_vtable = {
		.hit = (void*)&moving_sphere_hit,
		.bounding_box  = (void*)&moving_sphere_bounding_box
	};
	result->parent.vtable = &moving_sphere_vtable;

	return (void*)result;
}
