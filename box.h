typedef struct box
{
	hittable parent;
	float3 box_min;
	float3 box_max;
	hittable *sides;
} box;

internal bool box_hit(box const *this, ray const *r, float t0, float t1, hit_record *rec)
{
	return this->sides->vtable->hit(this->sides, r, t0, t1, rec);
}

internal bool box_bounding_box(box const *this, float t0, float t1, aabb *output_box)
{
	*output_box = (aabb){this->box_min, this->box_max};
	return true;
}

internal hittable *new_box(float3 p0, float3 p1, material *mat)
{
	box *result = malloc(sizeof(box));
	result->box_min = p0;
	result->box_max = p1;
	result->sides = new_hittable_list();

	/* add sides to the result box */
	hittable_list_add(result->sides, new_xy_rect(p0.x, p1.x, p0.y, p1.y, p1.z, mat));
	hittable_list_add(result->sides, new_xy_rect(p0.x, p1.x, p0.y, p1.y, p0.z, mat));

	hittable_list_add(result->sides, new_xz_rect(p0.x, p1.x, p0.z, p1.z, p1.y, mat));
	hittable_list_add(result->sides, new_xz_rect(p0.x, p1.x, p0.z, p1.z, p0.y, mat));
	
	hittable_list_add(result->sides, new_yz_rect(p0.y, p1.y, p0.z, p1.z, p1.x, mat));
	hittable_list_add(result->sides, new_yz_rect(p0.y, p1.y, p0.z, p1.z, p0.x, mat));

	/* setup vtable */
	static struct hittable_vtable box_vtable = {
		.hit = (void*)box_hit,
		.bounding_box = (void*)box_bounding_box
	};
	result->parent.vtable = &box_vtable;
	
	return (void*)result;
}
