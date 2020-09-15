typedef struct sphere
{
	hittable parent;
	float3 center;
	float radius;
	material *mat_ptr;	
} sphere;

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
			rec->mat_ptr = this->mat_ptr;
			return true;
		}
	}

	return false;
}

hittable *new_sphere(float3 center, float radius, void *m)
{
	sphere *result = malloc(sizeof(sphere));
	result->center = center;
	result->radius = radius;
	result->mat_ptr = m;
	
	/* setup vtable */
	static struct hittable_vtable sphere_vtable = {.hit = (void*)&sphere_hit};
	result->parent.vtable = &sphere_vtable;

	return (void*)result;
}
