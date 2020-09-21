typedef struct sphere
{
	hittable parent;
	float3 center;
	float radius;
	material *mat_ptr;	
} sphere;

internal void get_sphere_uv(float3 p, float2 *uv)
{
	float phi = atan2f(p.z, p.z);
	float theta = asinf(p.y);
	(*uv)[0] = 1 - (phi + PI) / (2 * PI);
	(*uv)[1] = (theta + PI / 2) / PI;
}

internal bool sphere_hit(sphere const *this, ray const *r, float t_min, float t_max, hit_record *rec)
{
	float3 oc = r->pos - this->center;
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
			float3 outward_normal = (rec->p - this->center) / this->radius;
			hit_record_set_face_normal(rec, r, outward_normal);
			get_sphere_uv((rec->p - this->center) / this->radius, &rec->uv);			
			rec->mat_ptr = this->mat_ptr;
			return true;
		}

		temp = (-half_b + root) / a;
		if(temp < t_max && temp > t_min)
		{
			rec->t = temp;
			rec->p = ray_at(r, rec->t);
			float3 outward_normal = (rec->p - this->center) / this->radius;
			hit_record_set_face_normal(rec, r, outward_normal);
			get_sphere_uv((rec->p - this->center) / this->radius, &rec->uv);
			rec->mat_ptr = this->mat_ptr;
			return true;
		}
	}

	return false;
}

internal bool sphere_bounding_box(sphere const *this, float t0, float t1, aabb *output_box)
{
	*output_box = (aabb){this->center - this->radius, this->center + this->radius};
	return true;
}

sphere make_sphere(float3 center, float radius, material *m)
{
    /* setup vtable */
    static struct hittable_vtable sphere_vtable = {
        .hit = (void*)&sphere_hit,
        .bounding_box = (void*)&sphere_bounding_box
    };

    return (sphere) {
	    .center = center,
	    .radius = radius,
	    .mat_ptr = m,
	    .parent.vtable = &sphere_vtable
	};
}
