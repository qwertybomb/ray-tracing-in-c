
#pragma once
struct material;

struct material_vtable
{
	bool(*scatter)(struct material const *this, ray const *r_in, hit_record const *rec, float3 *attenuation, ray *scattered);
};

typedef struct material
{
	struct material_vtable *vtable;
} material;

typedef struct lambertian
{
	material parent;
	float3 albedo;
} lambertian;

bool lambertian_scatter(lambertian const *this, ray const *r_in, hit_record *const rec, float3 *attenuation, ray *scattered)
{
	float3 scatter_dir = rec->normal + random_unit_vector();
	*scattered = (ray){rec->p, scatter_dir};
	*attenuation = this->albedo;
	return true;
}

material *new_lambertian(float3 color)
{
	lambertian *result = malloc(sizeof(lambertian));
	result->albedo = color;
	
	static struct material_vtable lambertian_vtable = { .scatter = (void*)&lambertian_scatter };
	result->parent.vtable = &lambertian_vtable;
	return (void*)result;
}

typedef struct metal
{
	material parent;
	float3 albedo;
	float fuzz;
} metal;

bool metal_scatter(metal const *this, ray const *r_in, hit_record *const rec, float3 *attenuation, ray *scattered)
{
	float3 reflected = reflect(normalize(r_in->dir), rec->normal);
	*scattered = (ray){rec->p, reflected + this->fuzz * random_in_unit_sphere()};
	*attenuation = this->albedo;
	return (dot(scattered->dir, rec->normal) > 0);
}

material *new_metal(float3 color, float fuzz)
{
	metal *result = malloc(sizeof(metal));
	result->albedo = color;
	result->fuzz = fuzz < 1 ? fuzz : 1;

	static struct material_vtable metal_vtable = { .scatter = (void*)&metal_scatter };
	result->parent.vtable = &metal_vtable;
	return (void*)result;
}

typedef struct dielectric
{
	material parent;
	float ref_idx;
} dielectric;

internal inline float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 *= r0;
	return r0 + (1 - r0) * powf(1 - cosine, 5);
}

bool dielectric_scatter(dielectric const *this, ray const *r_in, hit_record *const rec, float3 *attenuation, ray *scattered)
{
	*attenuation = 1.0f;
	float etai_over_etat = rec->front_face ? 1.0f / this->ref_idx : this->ref_idx;

	float3 unit_direction = normalize(r_in->dir);

	float cos_theta = fminf(dot(-unit_direction, rec->normal), 1.0f);
	float sin_theta = sqrtf(1.0f - cos_theta * cos_theta);
	if(etai_over_etat * sin_theta > 1.0f)
	{
		float3 reflected = reflect(unit_direction, rec->normal);
		*scattered = (ray){rec->p, reflected};
		return true;
	}
	
	float reflect_prob  = schlick(cos_theta, etai_over_etat);
	if(randomf() < reflect_prob)
	{
		float3 reflected = reflect(unit_direction, rec->normal);
		*scattered = (ray){rec->p, reflected};
		return true;
				
	}
	
	float3 refracted = refract(unit_direction, rec->normal, etai_over_etat);
	*scattered = (ray){rec->p, refracted};
	return true;
}

material *new_dielectric(float ref_idx)
{
	dielectric *result = malloc(sizeof(dielectric));
	result->ref_idx = ref_idx;

	static struct material_vtable dielectric_vtable = { .scatter = (void*)&dielectric_scatter };
	result->parent.vtable = &dielectric_vtable;
	return (void*)result;
}
