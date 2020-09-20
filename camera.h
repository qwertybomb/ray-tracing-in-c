#pragma once

typedef struct camera
{
	float3 origin;
	float3 horizontal;
	float3 vertical;
	float3 lower_left_corner;
	float3 u, v, w;
	float lens_radius;
	float time0, time1;
} camera;

internal inline ray camera_get_ray(camera const *this, float s, float t)
{
	float3 rd = this->lens_radius * random_in_unit_disk();
	float3 offset = this->u * rd.x + this->v * rd.y;

	return (ray){
		this->origin + offset,
		this->lower_left_corner + s * this->horizontal + t * this->vertical - this->origin - offset,
		randomf(this->time0, this->time1)
	};
}

internal camera make_camera(float3 lookfrom, float3 lookat, float3 vup, float fov, float aspect_ratio, float aperture, float focus_dist, float t0, float t1)
{
	float theta = degrees_to_radians(fov);
	float h = tanf(theta / 2);
	float viewport_height = 2.0f * h;
	float viewport_width = aspect_ratio * viewport_height;

	float3 w = normalize(lookfrom - lookat);
	float3 u = normalize(cross(vup, w));
	float3 v = cross(w, u);
	camera result = {
		.w = w,
		.u = u,
		.v = v,
		.origin = lookfrom,
		.horizontal = focus_dist * viewport_width * u,
		.vertical= focus_dist * viewport_height * v,
		.lower_left_corner = result.origin - result.horizontal / 2 - result.vertical / 2 - w * focus_dist,
		.lens_radius = aperture / 2,
		.time0 = t0,
		.time1 = t1
	};

	return result;		
}
