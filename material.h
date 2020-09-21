#pragma once
struct material;

struct material_vtable
{
	bool(*scatter)(struct material const *this, ray const *r_in, hit_record const *rec, float3 *attenuation, ray *scattered);
    float3(*emitted)(struct material const *this, float2 uv, float3 p);
};

typedef struct material
{
	struct material_vtable *vtable;
} material;

internal float3 material_emitted(material const *this, float2 uv, float3 p)
{
    (void)this, (void)uv, (void)p;
    return 0;
}

internal bool material_scatter(material const *this, ray const *r_in, hit_record const *rec, float3 *attenuation, ray *scattered)
{
    return false;
}

typedef struct lambertian
{
	material parent;
	texture *albedo;
} lambertian;

internal bool lambertian_scatter(lambertian const *this, ray const *r_in, hit_record const *rec, float3 *attenuation, ray *scattered)
{
	float3 scatter_direction = rec->normal + random_in_unit_sphere();
	*scattered = (ray){rec->p, scatter_direction, r_in->time};
	*attenuation = this->albedo->vtable->value(this->albedo, rec->uv, rec->p);
	return true;
}

__attribute__((overloadable)) internal material *new_lambertian(texture *color)
{
	lambertian *result = malloc(sizeof(lambertian));
	result->albedo = color;
	
	static struct material_vtable lambertian_vtable = {
	    .scatter = (void*)&lambertian_scatter,
	    .emitted = &material_emitted
	};
	result->parent.vtable = &lambertian_vtable;
	return (void*)result;
}

__attribute__((overloadable)) internal material *new_lambertian(float3 color)
{
	return new_lambertian(new_solid_color(color));
}

typedef struct metal
{
	material parent;
	float3 albedo;
	float fuzz;
} metal;

internal bool metal_scatter(metal const *this, ray const *r_in, hit_record const *rec, float3 *attenuation, ray *scattered)
{
	float3 reflected = reflect(normalize(r_in->dir), rec->normal);
	*scattered = (ray){rec->p, reflected + this->fuzz * random_in_unit_sphere()};
	*attenuation = this->albedo;
	return (dot(scattered->dir, rec->normal) > 0);
}

internal material *new_metal(float3 color, float fuzz)
{
	metal *result = malloc(sizeof(metal));
	result->albedo = color;
	result->fuzz = fuzz < 1 ? fuzz : 1;

	static struct material_vtable metal_vtable = {
	    .scatter = (void*)&metal_scatter,
	    .emitted = (void*)&material_emitted
	};
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

internal bool dielectric_scatter(dielectric const *this, ray const *r_in, hit_record const *rec, float3 *attenuation, ray *scattered)
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

internal material *new_dielectric(float ref_idx)
{
	dielectric *result = malloc(sizeof(dielectric));
	result->ref_idx = ref_idx;

	static struct material_vtable dielectric_vtable = {
	    .scatter = (void*)&dielectric_scatter,
	    .emitted = &material_emitted
	};
	result->parent.vtable = &dielectric_vtable;
	return (void*)result;
}

typedef struct diffuse_light
{
	material parent;
	texture *emit;
} diffuse_light;

internal float3 diffuse_light_emitted(diffuse_light const *this, float2 uv, float3 p)
{
    return this->emit->vtable->value(this->emit, uv, p);
}

__attribute__((overloadable)) internal material *new_diffuse_light(texture *a)
{
	diffuse_light *result = malloc(sizeof(diffuse_light));
	result->emit = a;

	/* setup vtable */
	static struct material_vtable diffuse_light_vtable = {
		.scatter = &material_scatter,
		.emitted = (void*)&diffuse_light_emitted
	};
	result->parent.vtable = &diffuse_light_vtable;

	return (void*)result;
}

__attribute__((overloadable)) internal material *new_diffuse_light(float3 c)
{
	return new_diffuse_light(new_solid_color(c));
}

typedef struct isotropic
{
    material parent;
    texture *albedo;
} isotropic;

internal bool isotropic_scatter(isotropic const *this, ray const *r, hit_record const *rec, float3 *attenuation, ray *scattered)
{
    *scattered = (ray){rec->p, random_in_unit_sphere(), r->time};
    *attenuation = this->albedo->vtable->value(this->albedo, rec->uv, rec->p);
    return true;
}

__attribute__((overloadable)) internal material *new_isotropic(texture *a)
{
    isotropic *result = malloc(sizeof(isotropic));
    result->albedo = a;

    /* setup vtable */
    static struct material_vtable isotropic_vtable = {
        .scatter = (void*)&isotropic_scatter,
        .emitted = &material_emitted
    };
    result->parent.vtable = &isotropic_vtable;

    return (void*)result;
}